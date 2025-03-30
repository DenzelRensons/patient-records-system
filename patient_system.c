#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PATIENTS 50
#define MAX_NAME_LEN 100
#define MAX_HISTORY_LEN 500

typedef struct {
    int id;
    char* name;
    int age;
    char gender;
    char* medical_history;
    char admission_date[20];
    char discharge_date[20];
    bool is_discharged;
} Patient;

typedef struct {
    Patient* patients;
    size_t size;
    size_t capacity;
} PatientDatabase;

// Function prototypes
PatientDatabase* create_database(size_t initial_capacity);
bool add_patient(PatientDatabase* db);
bool update_patient(PatientDatabase* db);
bool remove_patient(PatientDatabase* db);
void display_patient(const Patient* p);
void display_all_patients(const PatientDatabase* db, bool show_discharged);
bool save_to_file(const PatientDatabase* db);
void free_database(PatientDatabase* db);
void print_menu();
void get_current_datetime(char* buffer);
void clear_input_buffer();

// Main program
int main() {
    PatientDatabase* db = create_database(5);
    if (!db) {
        fprintf(stderr, "Failed to initialize database\n");
        return EXIT_FAILURE;
    }

    int choice;
    do {
        print_menu();
        printf("Enter your choice (1-8): ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                if (!add_patient(db)) {
                    printf("Failed to add patient!\n");
                }
                break;
            case 2:
                if (!update_patient(db)) {
                    printf("Failed to update patient!\n");
                }
                break;
            case 3:
                if (!remove_patient(db)) {
                    printf("Failed to remove patient!\n");
                }
                break;
            case 4: {
                printf("Enter patient ID: ");
                int id;
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    bool found = false;
                    for (size_t i = 0; i < db->size; i++) {
                        if (db->patients[i].id == id) {
                            display_patient(&db->patients[i]);
                            found = true;
                            break;
                        }
                    }
                    if (!found) printf("Patient not found!\n");
                } else {
                    clear_input_buffer();
                    printf("Invalid ID!\n");
                }
                break;
            }
            case 5:
                display_all_patients(db, false);
                break;
            case 6:
                display_all_patients(db, true);
                break;
            case 7:
                if (save_to_file(db)) {
                    printf("Data saved successfully!\n");
                } else {
                    printf("Failed to save data!\n");
                }
                break;
            case 8:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please enter 1-8.\n");
        }
    } while (choice != 8);

    free_database(db);
    return EXIT_SUCCESS;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

PatientDatabase* create_database(size_t initial_capacity) {
    PatientDatabase* db = (PatientDatabase*)malloc(sizeof(PatientDatabase));
    if (!db) return NULL;

    db->patients = (Patient*)calloc(initial_capacity, sizeof(Patient));
    if (!db->patients) {
        free(db);
        return NULL;
    }

    db->size = 0;
    db->capacity = initial_capacity;
    return db;
}

bool add_patient(PatientDatabase* db) {
    if (db->size >= MAX_PATIENTS) {
        printf("Database full! Maximum %d patients reached.\n", MAX_PATIENTS);
        return false;
    }

    if (db->size >= db->capacity) {
        size_t new_capacity = (db->capacity * 2 > MAX_PATIENTS) ? MAX_PATIENTS : db->capacity * 2;
        Patient* new_patients = (Patient*)realloc(db->patients, new_capacity * sizeof(Patient));
        if (!new_patients) return false;
        db->patients = new_patients;
        db->capacity = new_capacity;
    }

    Patient* p = &db->patients[db->size];
    
    printf("Enter patient ID (must be unique and positive): ");
    if (scanf("%d", &p->id) != 1 || p->id <= 0) {
        clear_input_buffer();
        printf("Invalid ID! Must be positive number.\n");
        return false;
    }
    clear_input_buffer();
    
    // Check if ID already exists
    for (size_t i = 0; i < db->size; i++) {
        if (db->patients[i].id == p->id && !db->patients[i].is_discharged) {
            printf("Patient ID already exists!\n");
            return false;
        }
    }
    
    printf("Enter patient name: ");
    char name[MAX_NAME_LEN];
    if (scanf(" %99[^\n]", name) != 1) {
        clear_input_buffer();
        printf("Invalid name!\n");
        return false;
    }
    p->name = strdup(name);
    
    printf("Enter patient age (1-120): ");
    if (scanf("%d", &p->age) != 1 || p->age < 1 || p->age > 120) {
        clear_input_buffer();
        free(p->name);
        printf("Invalid age! Must be 1-120.\n");
        return false;
    }
    clear_input_buffer();
    
    printf("Enter gender (M/F/O): ");
    if (scanf(" %c", &p->gender) != 1 || 
        (p->gender != 'M' && p->gender != 'F' && p->gender != 'O')) {
        clear_input_buffer();
        free(p->name);
        printf("Invalid gender! Must be M, F, or O.\n");
        return false;
    }
    clear_input_buffer();
    
    printf("Enter initial medical history: ");
    char history[MAX_HISTORY_LEN];
    if (scanf(" %499[^\n]", history) != 1) {
        clear_input_buffer();
        free(p->name);
        printf("Invalid medical history!\n");
        return false;
    }
    p->medical_history = strdup(history);
    
    get_current_datetime(p->admission_date);
    strcpy(p->discharge_date, "Not discharged");
    p->is_discharged = false;

    if (!p->name || !p->medical_history) {
        free(p->name);
        free(p->medical_history);
        return false;
    }

    db->size++;
    printf("\nPatient added successfully!\n");
    display_patient(p);
    return true;
}

bool update_patient(PatientDatabase* db) {
    printf("Enter patient ID to update: ");
    int id;
    if (scanf("%d", &id) != 1) {
        clear_input_buffer();
        printf("Invalid ID!\n");
        return false;
    }
    clear_input_buffer();

    for (size_t i = 0; i < db->size; i++) {
        if (db->patients[i].id == id && !db->patients[i].is_discharged) {
            printf("\nCurrent patient details:\n");
            display_patient(&db->patients[i]);
            
            printf("\nEnter new medical history: ");
            char history[MAX_HISTORY_LEN];
            if (scanf(" %499[^\n]", history) != 1) {
                clear_input_buffer();
                printf("Invalid medical history!\n");
                return false;
            }
            clear_input_buffer();
            
            free(db->patients[i].medical_history);
            db->patients[i].medical_history = strdup(history);
            
            if (!db->patients[i].medical_history) {
                printf("Failed to update medical history!\n");
                return false;
            }
            
            printf("\nPatient record updated successfully!\n");
            display_patient(&db->patients[i]);
            return true;
        }
    }
    
    printf("Patient not found or already discharged!\n");
    return false;
}

bool remove_patient(PatientDatabase* db) {
    printf("Enter patient ID to discharge: ");
    int id;
    if (scanf("%d", &id) != 1) {
        clear_input_buffer();
        printf("Invalid ID!\n");
        return false;
    }
    clear_input_buffer();

    for (size_t i = 0; i < db->size; i++) {
        if (db->patients[i].id == id && !db->patients[i].is_discharged) {
            db->patients[i].is_discharged = true;
            get_current_datetime(db->patients[i].discharge_date);
            
            printf("\nPatient discharged successfully!\n");
            display_patient(&db->patients[i]);
            
            // Optional: Actually free memory and remove from array
            // But we'll keep them in the system with discharged status
            return true;
        }
    }
    
    printf("Patient not found or already discharged!\n");
    return false;
}

void display_patient(const Patient* p) {
    if (!p) {
        printf("Invalid patient record!\n");
        return;
    }
    
    printf("\n=== PATIENT DETAILS ===\n");
    printf("ID: %d\n", p->id);
    printf("Name: %s\n", p->name);
    printf("Age: %d\n", p->age);
    printf("Gender: %c\n", p->gender);
    printf("Medical History: %s\n", p->medical_history);
    printf("Admission Date: %s\n", p->admission_date);
    printf("Discharge Date: %s\n", p->discharge_date);
    printf("Status: %s\n\n", p->is_discharged ? "DISCHARGED" : "ACTIVE");
}

void display_all_patients(const PatientDatabase* db, bool show_discharged) {
    if (db->size == 0) {
        printf("\nNo patients in the system!\n");
        return;
    }
    
    printf("\n=== %s PATIENTS ===\n", show_discharged ? "ALL" : "ACTIVE");
    int count = 0;
    for (size_t i = 0; i < db->size; i++) {
        if (show_discharged || !db->patients[i].is_discharged) {
            display_patient(&db->patients[i]);
            count++;
        }
    }
    
    if (count == 0) {
        printf("No %s patients found.\n", show_discharged ? "" : "active");
    } else {
        printf("Total %s patients: %d\n", show_discharged ? "" : "active", count);
    }
}

bool save_to_file(const PatientDatabase* db) {
    FILE* file = fopen("patient_records.txt", "w");
    if (!file) return false;
    
    for (size_t i = 0; i < db->size; i++) {
        fprintf(file, "ID: %d\n", db->patients[i].id);
        fprintf(file, "Name: %s\n", db->patients[i].name);
        fprintf(file, "Age: %d\n", db->patients[i].age);
        fprintf(file, "Gender: %c\n", db->patients[i].gender);
        fprintf(file, "Medical History: %s\n", db->patients[i].medical_history);
        fprintf(file, "Admission Date: %s\n", db->patients[i].admission_date);
        fprintf(file, "Discharge Date: %s\n", db->patients[i].discharge_date);
        fprintf(file, "Status: %s\n\n", 
                db->patients[i].is_discharged ? "Discharged" : "Active");
    }
    
    fclose(file);
    return true;
}

void free_database(PatientDatabase* db) {
    if (!db) return;

    for (size_t i = 0; i < db->size; i++) {
        free(db->patients[i].name);
        free(db->patients[i].medical_history);
    }

    free(db->patients);
    free(db);
}

void print_menu() {
    printf("\n=== HOSPITAL PATIENT RECORDS SYSTEM ===\n");
    printf("1. Add new patient\n");
    printf("2. Update patient record\n");
    printf("3. Discharge patient\n");
    printf("4. View patient details\n");
    printf("5. View all active patients\n");
    printf("6. View all patients (including discharged)\n");
    printf("7. Save data to file\n");
    printf("8. Exit\n");
}

void get_current_datetime(char* buffer) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, 20, "%Y-%m-%d %H:%M", tm_info);
}
