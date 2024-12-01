#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple date struct
typedef struct Date
{
    int day;
    int month;
    int year;
} Date;

// Individual student data
typedef struct StudentData
{
    char  nisn[11];
    char *name;
    char  sex;
    char *birth_place;
    Date  birth_date;

    char *address;
    char *phone_number;

    char *father_name;
    char *father_phone;

    char *mother_name;
    char *mother_phone;

    char *guardian_name;
    char *guardian_phone;
} StudentData;

// Collection type
typedef struct StudentEntry
{
    StudentData          data;
    struct StudentEntry *next;
} StudentEntry;

// Allocate string, deallocate old string if it exists
void allocateString(char **output_ptr, const char *str)
{
    if (*output_ptr != NULL)
    {
        free(*output_ptr);
        *output_ptr = NULL;
    }

    *output_ptr = calloc(strlen(str) + 1, sizeof(char));
    strcpy(*output_ptr, str);
}

// Clear input buffer (newline and all)
void clearInput()
{
    if (feof(stdin)) return;

    int c;
    do c = getchar();
    while (c != '\n' && c != EOF);
}

// Get user input with optional filter and fallback value
void getInput(char *output, int max_length, const char *filter, const char *fallback)
{
    int  success = 0;
    char buffer[1024];

    do
    {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (fallback != NULL && strlen(buffer) == 0)
        {
            strncpy(output, fallback, max_length);
            break;
        }

        if (filter != NULL && strlen(buffer) > 0)
            success = strspn(buffer, filter) == strlen(buffer);
        else if (filter == NULL)
            success = strlen(buffer) > 0;

        if (!success && strlen(buffer) > 0)
            printf("Input tidak valid, coba lagi\n");
        else if (success)
            strncpy(output, buffer, max_length);

    } while (success == 0);
}

// Get user input but dynamically allocate string to fit size
void getInputDynamic(char **output_ptr, const char *filter, const char *fallback)
{
    char buffer[1024] = {0};

    getInput(buffer, sizeof(buffer) - 1, filter, fallback);
    allocateString(output_ptr, buffer);
}

// Get user input for dates
void getInputDate(Date *output)
{
    int new_day   = 0;
    int new_month = 0;
    int new_year  = 0;
    int max_day   = 31;
    int match     = 1;
    do
    {
        match = 1;

        scanf("%d-%d-%d", &new_day, &new_month, &new_year);
        clearInput();

        if (new_month == 4 || new_month == 6 || new_month == 9 || new_month == 11)
            max_day = 30;

        if (new_month == 2)
        {
            max_day = 28;
            if (new_year % 4 == 0 && (new_year % 400 == 0 || new_year % 100 != 0))
                max_day = 29;
        }

        match &= new_month < 13 && new_month > 0;
        match &= new_day <= max_day && new_day > 0;

        if (!match)
            printf("Input tidak valid, joba lagi\n");
        else
            *output = (Date){new_day, new_month, new_year};

    } while (match == 0);
}

// Get user input for phone numbers, with optional fallback value
void getInputPhone(char **output_ptr, const char *fallback)
{
    char buffer[32];
    int  match = 0;
    do
    {
        getInput(buffer, sizeof(buffer) - 1, "+0987654321", fallback);

        // Quick exit out of a fallback input
        if (strcmp(buffer, fallback) == 0)
            break;

        // Don't let user input '+' except as a prefix at the start
        match = strchr(buffer + 1, '+') == NULL;
    } while (match == 0);

    allocateString(output_ptr, buffer);
}

// Find entry for student using their NISN
StudentEntry *findStudent(StudentEntry *head, const char *nisn)
{
    StudentEntry *result = NULL;
    StudentEntry *curr   = head;

    while (curr != NULL)
    {
        if (strcmp(curr->data.nisn, nisn) == 0)
        {
            result = curr;
            break;
        }
        curr = curr->next;
    }

    return result;
}

// Add new student entry
StudentEntry *addStudent(StudentEntry *head)
{
    StudentData   new_data  = {0};
    StudentEntry *new_entry = NULL;

    printf("--- Tambah Data Siswa ---\n");
    printf("NISN Siswa:\n");
    getInput(new_data.nisn, 10, "0123456789", NULL);

    if (findStudent(head, new_data.nisn) != NULL)
    {
        printf("Maaf, siswa dengan NISN %s sudah terdaftar\n", new_data.nisn);
        return head;
    }

    printf("Nama Siswa:\n");
    getInputDynamic(&new_data.name, NULL, NULL);

    printf("Jenis Kelamin Siswa (L/P):\n");
    getInput(&new_data.sex, 1, "lpLP", NULL);
    new_data.sex = toupper(new_data.sex);

    printf("Tempat Lahir Siswa:\n");
    getInputDynamic(&new_data.birth_place, NULL, NULL);

    printf("Tanggal Lahir Siswa (DD-MM-YYYY):\n");
    getInputDate(&new_data.birth_date);

    printf("Alamat Rumah Siswa:\n");
    getInputDynamic(&new_data.address, NULL, NULL);

    printf("Nomor Telpon Siswa:\n");
    getInputPhone(&new_data.phone_number, "-");

    printf("Nama Ayah Siswa:\n");
    getInputDynamic(&new_data.father_name, NULL, "-");
    if (strcmp(new_data.father_name, "-") != 0)
    {
        printf("Nomor Telpon Ayah Siswa:\n");
        getInputPhone(&new_data.father_phone, "-");
    }
    else
        allocateString(&new_data.father_phone, "-");

    printf("Nama Ibu Siswa:\n");
    getInputDynamic(&new_data.mother_name, NULL, "-");
    if (strcmp(new_data.mother_name, "-") != 0)
    {
        printf("Nomor Telpon Ibu Siswa:\n");
        getInputPhone(&new_data.mother_phone, "-");
    }
    else
        allocateString(&new_data.mother_phone, "-");

    printf("Nama Wali Siswa:\n");
    getInputDynamic(&new_data.guardian_name, NULL, "-");
    if (strcmp(new_data.guardian_name, "-") != 0)
    {
        printf("Nomor Telpon Wali Siswa:\n");
        getInputPhone(&new_data.guardian_phone, "-");
    }
    else
        allocateString(&new_data.guardian_phone, "-");

    new_entry       = (StudentEntry *)calloc(1, sizeof(StudentEntry));
    new_entry->next = head;
    new_entry->data = new_data;

    printf("Data siswa berhasil ditambah\n");

    return new_entry;
}

// Delete student based on their NISN from entries
StudentEntry *deleteStudent(StudentEntry *head, const char *nisn)
{
    StudentEntry *new_head = head;

    StudentEntry *prev = NULL;
    StudentEntry *curr = head;

    while (head != NULL)
    {
        if (strcmp(curr->data.nisn, nisn) == 0)
        {
            // Erase data
            if (prev == NULL)
                new_head = head->next;
            else
                prev->next = curr->next;

            free(curr->data.name);
            free(curr->data.birth_place);
            free(curr->data.address);
            free(curr->data.phone_number);

            free(curr->data.father_name);
            free(curr->data.father_phone);

            free(curr->data.mother_name);
            free(curr->data.mother_phone);

            free(curr->data.guardian_name);
            free(curr->data.guardian_phone);

            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    return new_head;
}

// Edit student data
StudentEntry *editStudent(StudentEntry *head)
{
    StudentEntry *new_head     = head;
    StudentEntry *target_entry = NULL;
    char          nisn_buffer[11];
    int           choice = 0;

    printf("--- Edit Data Siswa ---\n");
    printf("NISN Siswa:\n");
    getInput(nisn_buffer, 10, "0123456789", NULL);

    target_entry = findStudent(head, nisn_buffer);
    if (target_entry == NULL)
    {
        printf("Siswa dengan NISN %s tidak ditemukan\n", nisn_buffer);
        return head;
    }

    printf("Mengedit Data\n");
    printf("NISN: %s\n", target_entry->data.nisn);
    printf("Nama: %s\n", target_entry->data.name);
    printf("Data yang ingin diedit\n");
    printf("[1] Nama               : %s\n", target_entry->data.name);
    printf("[2] Tempat Lahir       : %s\n", target_entry->data.birth_place);
    printf("[3] Tanggal Lahir      : %d-%d-%d\n",
           target_entry->data.birth_date.day,
           target_entry->data.birth_date.month,
           target_entry->data.birth_date.year);
    printf("[4] Alamat             : %s\n", target_entry->data.address);
    printf("[5] Nomor Telpon       : %s\n", target_entry->data.phone_number);
    printf("\n");
    printf("[6] Nama Ayah          : %s\n", target_entry->data.father_name);
    printf("[7] Nomor Telpon Ayah  : %s\n", target_entry->data.father_phone);
    printf("\n");
    printf("[8] Nama Ibu           : %s\n", target_entry->data.mother_name);
    printf("[9] Nomor Telpon Ibu   : %s\n", target_entry->data.mother_phone);
    printf("\n");
    printf("[10] Nama Wali         : %s\n", target_entry->data.guardian_name);
    printf("[11] Nomor Telpon Wali : %s\n", target_entry->data.guardian_phone);
    printf("\n");
    printf("[12] Hapus Data Siswa\n");
    printf("[13] Batal\n");
    printf("---\n");

    scanf("%d", &choice);
    clearInput();

    int had_father   = strcmp(target_entry->data.father_name, "-") != 0;
    int had_mother   = strcmp(target_entry->data.mother_name, "-") != 0;
    int had_guardian = strcmp(target_entry->data.guardian_name, "-") != 0;

    switch (choice)
    {
        case 1:
            printf("Masukkan Nama Baru Siswa:\n");
            getInputDynamic(&target_entry->data.name, NULL, NULL);
            break;
        case 2:
            printf("Masukkan Tempat Lahir Baru Siswa:\n");
            getInputDynamic(&target_entry->data.birth_place, NULL, NULL);
            break;
        case 3:
            printf("Masukkan Tanggal Lahir Baru Siswa:\n");
            getInputDate(&target_entry->data.birth_date);
            break;
        case 4:
            printf("Masukkan Alamat Baru Siswa:\n");
            getInputDynamic(&target_entry->data.address, NULL, NULL);
            break;
        case 5:
            printf("Masukkan Nomor Telpon Baru Siswa:\n");
            getInputPhone(&target_entry->data.phone_number, "-");
            break;
        case 6:
            printf("Masukkan Nama Baru Ayah Siswa (Kosongkan untuk menghapus):\n");
            getInputDynamic(&target_entry->data.father_name, NULL, "-");

            // If father now doesn't exist, remove phone number
            if (had_father && strcmp(target_entry->data.father_name, "-") == 0)
                allocateString(&target_entry->data.father_phone, "-");
            break;
        case 7:
            // Father doesn't exist yet, get that first
            if (!had_father)
            {
                printf("Masukkan Nama Baru Ayah Siswa:\n");
                getInputDynamic(&target_entry->data.father_name, NULL, NULL);
            }

            printf("Masukkan Nomor Telpon Baru Ayah Siswa:\n");
            getInputPhone(&target_entry->data.father_phone, "-");
            break;
        case 8:
            printf("Masukkan Nama Baru Ibu Siswa (Kosongkan untuk menghapus):\n");
            getInputDynamic(&target_entry->data.mother_name, NULL, "-");

            // If mother now doesn't exist, remove phone number
            if (had_mother && strcmp(target_entry->data.mother_name, "-") == 0)
                allocateString(&target_entry->data.mother_phone, "-");
            break;
        case 9:
            // Mother doesn't exist yet, get that first
            if (!had_mother)
            {
                printf("Masukkan Nama Baru Ibu Siswa:\n");
                getInputDynamic(&target_entry->data.mother_name, NULL, NULL);
            }

            printf("Masukkan Nomor Telpon Baru Ibu Siswa:\n");
            getInputPhone(&target_entry->data.mother_phone, "-");
            break;
        case 10:
            printf("Masukkan Nama Baru Wali Siswa (Kosongkan untuk menghapus):\n");
            getInputDynamic(&target_entry->data.guardian_name, NULL, "-");

            // If guardian now doesn't exist, remove phone number
            if (had_guardian && strcmp(target_entry->data.guardian_name, "-") == 0)
                allocateString(&target_entry->data.guardian_phone, "-");
            break;
        case 11:
            // Guardian doesn't exist yet, get that first
            if (!had_guardian)
            {
                printf("Masukkan Nama Baru Wali Siswa:\n");
                getInputDynamic(&target_entry->data.guardian_name, NULL, NULL);
            }

            printf("Masukkan Nomor Telpon Baru Wali Siswa:\n");
            getInputPhone(&target_entry->data.guardian_phone, "-");
            break;

        case 12: new_head = deleteStudent(head, nisn_buffer); break;
        case 13: /* Do Nothing */ break;
        default: printf("Pilihan Tidak Valid\n"); break;
    }

    if (choice < 12)
        printf("Data siswa berhasil diedit\n");
    else if (choice == 12)
        printf("Data siswa berhasil dihapus\n");

    return new_head;
}

// Display all data of a student
void viewStudent(StudentEntry *head)
{
    StudentEntry *student = NULL;
    char          nisn_buffer[11];

    printf("Masukkan NISN Siswa:\n");
    getInput(nisn_buffer, 10, "0123456789", NULL);

    student = findStudent(head, nisn_buffer);
    if (student == NULL)
    {
        printf("Siswa dengan NISN %s tidak ditemukan\n", nisn_buffer);
        return;
    }

    printf("--- Data Siswa ---\n");
    printf("NISN           : %s\n", student->data.nisn);
    printf("Nama           : %s\n", student->data.name);
    printf("Jenis Kelamin  : %c\n", student->data.sex);
    printf("Tempat Lahir   : %s\n", student->data.birth_place);
    printf("Tanggal Lahir  : %d-%d-%d\n",
           student->data.birth_date.day,
           student->data.birth_date.month,
           student->data.birth_date.year);
    printf("Alamat Rumah   : %s\n", student->data.address);
    printf("Nomor Telpon   : %s\n", student->data.phone_number);
    printf("\n");
    printf("Nama Ayah      : %s\n", student->data.father_name);
    if (strcmp(student->data.father_name, "-") != 0)
        printf("Telpon Ayah    : %s\n", student->data.father_phone);
    printf("\n");
    printf("Nama Ibu       : %s\n", student->data.mother_name);
    if (strcmp(student->data.mother_name, "-") != 0)
        printf("Telpon Ibu     : %s\n", student->data.mother_phone);
    printf("\n");
    if (strcmp(student->data.guardian_name, "-") != 0)
        printf("Nama Wali      : %s\n", student->data.guardian_name);
    printf("Telpon Wali    : %s\n", student->data.guardian_phone);
}

// Free student entry list
void clearStudents(StudentEntry *head)
{
    StudentEntry *curr = head;
    while (curr != NULL) curr = deleteStudent(curr, curr->data.nisn);
}

/*
 * Load Date -> store loaded file, no default
 * Save Data -> default path is last loaded file if exists,
 *              otherwise a default path with conflict mitigation
 */

// Load student data collection from file
StudentEntry *loadData(char **loaded_file_ptr)
{
    StudentEntry *result     = NULL;
    FILE         *input_file = NULL;
    char          path_buffer[128];
    char          buffer[1024];

    printf("--- Muat Data ---\n");
    printf("Lokasi File:\n");
    getInput(path_buffer, sizeof(path_buffer) - 1, NULL, NULL);

    input_file = fopen(path_buffer, "r");
    if (input_file == NULL)
    {
        printf("File %s tidak ditemukan, gagal membaca file\n", path_buffer);
        return result;
    }

    // File header
    fgets(buffer, sizeof(buffer), input_file);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strcmp(buffer, "PDSiswa1") != 0)
    {
        printf("Peringatan! Header file %s tidak sesuai, data mungkin salah\n",
               path_buffer);
    }

    // NISN
    while (fscanf(input_file, "%10[^\n]\n", buffer) != EOF)
    {
        StudentEntry *temp = (StudentEntry *)calloc(1, sizeof(StudentEntry));
        temp->data         = (StudentData){0};

        temp->next = result;
        result     = temp;

        strncpy(temp->data.nisn, buffer, 10);

        fscanf(input_file, "%1023[^\n]\n", buffer);
        allocateString(&temp->data.name, buffer);

        fscanf(input_file, "%c\n", &temp->data.sex);

        fscanf(input_file, "%1023[^\n]\n", buffer);
        allocateString(&temp->data.birth_place, buffer);

        fscanf(input_file, "%d-%d-%d\n",
               &temp->data.birth_date.day,
               &temp->data.birth_date.month,
               &temp->data.birth_date.year);

        fscanf(input_file, "%1023[^\n]\n", buffer);
        allocateString(&temp->data.address, buffer);

        fscanf(input_file, "%31[^\n]\n", buffer);
        allocateString(&temp->data.phone_number, buffer);

        fscanf(input_file, "%1023[^\n]\n", buffer);
        allocateString(&temp->data.father_name, buffer);

        fscanf(input_file, "%31[^\n]\n", buffer);
        allocateString(&temp->data.father_phone, buffer);

        fscanf(input_file, "%1023[^\n]\n", buffer);
        allocateString(&temp->data.mother_name, buffer);

        fscanf(input_file, "%31[^\n]\n", buffer);
        allocateString(&temp->data.mother_phone, buffer);

        fscanf(input_file, "%1023[^\n]\n", buffer);
        allocateString(&temp->data.guardian_name, buffer);

        fscanf(input_file, "%31[^\n]\n", buffer);
        allocateString(&temp->data.guardian_phone, buffer);
    }

    // Correct ordering by reversing list
    StudentEntry *curr = result;
    StudentEntry *prev = NULL;

    while (curr != NULL)
    {
        StudentEntry *temp_next = curr->next;

        curr->next = prev;
        prev       = curr;
        curr       = temp_next;
    }

    result = prev;

    if (loaded_file_ptr != NULL)
        allocateString(loaded_file_ptr, path_buffer);

    fclose(input_file);
    printf("Data berhasil dimuat\n");

    return result;
}

// Save student data collection to file
void saveData(StudentEntry *head, char *last_loaded_file)
{
    FILE *output_file;
    char  path_buffer[128];
    char  suggested_path[128];
    char  choice;

    StudentEntry *curr = head;

    // Suggest already loaded file
    if (last_loaded_file != NULL)
        strncpy(suggested_path, last_loaded_file, sizeof(suggested_path));
    else
    {
        strcpy(suggested_path, "data_siswa.irf");

        // Check if default file exists and append number after
        output_file = fopen(suggested_path, "r");
        while (output_file != NULL)
        {
            fclose(output_file);
            if (strlen(suggested_path) > 120)
                break;

            strcpy(suggested_path + strlen(suggested_path) - 4, "_1.irf");
            output_file = fopen(suggested_path, "r");
        }
    }

    printf("--- Simpan Data ---\n");
    printf("Lokasi File (kosongkan untuk %s):\n", suggested_path);
    getInput(path_buffer, sizeof(path_buffer) - 1, NULL, suggested_path);

    output_file = fopen(path_buffer, "r");
    if (output_file != NULL)
    {
        // Check if file exists and close immediately
        fclose(output_file);

        // If path isn't the suggested path, warn for collision
        if (strcmp(path_buffer, suggested_path) != 0)
        {
            printf("File sudah ada, apakah anda ingin timpa dengan file baru? (Y/[T]):\n");
            getInput(&choice, 1, "yYtT", "T");

            if (toupper(choice) == 'T')
            {
                printf("Penyimpanan file batal\n");
                return;
            }
        }
    }

    output_file = fopen(path_buffer, "w");
    if (output_file == NULL)
    {
        printf("Gagal membuat file %s\n", path_buffer);
        return;
    }

    // File header
    fprintf(output_file, "PDSiswa1\n");

    while (curr != NULL)
    {
        fprintf(output_file, "%s\n", curr->data.nisn);
        fprintf(output_file, "%s\n", curr->data.name);
        fprintf(output_file, "%c\n", curr->data.sex);
        fprintf(output_file, "%s\n", curr->data.birth_place);
        fprintf(output_file, "%d-%d-%d\n",
                curr->data.birth_date.day,
                curr->data.birth_date.month,
                curr->data.birth_date.year);

        fprintf(output_file, "%s\n", curr->data.address);
        fprintf(output_file, "%s\n", curr->data.phone_number);

        fprintf(output_file, "%s\n", curr->data.father_name);
        fprintf(output_file, "%s\n", curr->data.father_phone);

        fprintf(output_file, "%s\n", curr->data.mother_name);
        fprintf(output_file, "%s\n", curr->data.mother_phone);

        fprintf(output_file, "%s\n", curr->data.guardian_name);
        fprintf(output_file, "%s\n", curr->data.guardian_phone);

        curr = curr->next;
    }

    fclose(output_file);
    printf("Data berhasil disimpan\n");
}

int main(void)
{
    // Collection of all student data
    StudentEntry *student_entries = NULL;

    int   choice            = 0;
    char *loaded_file_cache = NULL;

    printf("--- Selamat Datang di Sistem Pendataan Siswa ---\n\n");

    while (choice != 6)
    {
        printf("Menu Utama\n");
        printf("[1] Tambahkan data siswa\n");
        printf("[2] Edit data siswa\n");
        printf("[3] Tampilkan data siswa\n");
        printf("[4] Muat data siswa\n");
        printf("[5] Simpan data siswa\n");
        printf("[6] Keluar dari program\n");
        printf("Masukkan pilihan Anda: ");

        choice = 0;
        scanf("%d", &choice);
        clearInput();
        printf("\n");

        switch (choice)
        {
            case 1: student_entries = addStudent(student_entries); break;
            case 2: student_entries = editStudent(student_entries); break;
            case 3: viewStudent(student_entries); break;
            case 4:
                // Clean previously loaded data
                clearStudents(student_entries);
                student_entries = loadData(&loaded_file_cache);
                break;
            case 5: saveData(student_entries, loaded_file_cache); break;
            case 6: printf("Keluar dari program\n"); break;
            default: printf("Pilihan tidak valid\n"); break;
        }

        printf("\n");
    }

    clearStudents(student_entries);

    return 0;
}
