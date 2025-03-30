#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PATIENTS 50  // Maximum number of patients allowed

typedef struct {
    char* name;            // Dynamically allocated
    int id;
    char* medical_history; // Dynamically allocated
    size_t history_size;   // Tracks allocated size
} Patient;

typedef struct {
    Patient* patients;     // Dynamic array of patients
    size_t size;           // Current number of patients
    size_t capacity;       // Current array capacity
} PatientDatabase;

// Creates a new database with initial capacity
PatientDatabase* create_database(size_t initial_capacity) {
    PatientDatabase* db = (PatientDatabase*)malloc(sizeof(PatientDatabase));
    if (!db) {
        fprintf(stderr, "Failed to allocate database\n");
        return NULL;
    }

    db->patients = (Patient*)calloc(initial_capacity, sizeof(Patient));
    if (!db->patients) {
        free(db);
        return NULL;
    }

    db->size = 0;
    db->capacity = initial_capacity;
    return db;
}

// Adds a new patient if there's space
bool add_patient(PatientDatabase* db, const char* name, int id, const char* medical_history) {
    if (!db || !name || !medical_history) {
        fprintf(stderr, "Invalid NULL input\n");
        return false;
    }

    if (db->size >= MAX_PATIENTS) {
        fprintf(stderr, "Database full! Maximum %d patients reached.\n", MAX_PATIENTS);
        return false;
    }

    // Resize array if needed (without exceeding MAX_PATIENTS)
    if (db->size >= db->capacity) {
        size_t new_capacity = (db->capacity * 2 > MAX_PATIENTS) ? MAX_PATIENTS : db->capacity * 2;
        Patient* new_patients = (Patient*)realloc(db->patients, new_capacity * sizeof(Patient));
        if (!new_patients) {
            fprintf(stderr, "Failed to resize patient array\n");
            return false;
        }
        db->patients = new_patients;
        db->capacity = new_capacity;
    }

    // Store new patient data
    Patient* p = &db->patients[db->size];
    p->name = strdup(name);
    p->medical_history = strdup(medical_history);
    p->id = id;
    p->history_size = strlen(medical_history) + 1;

    if (!p->name || !p->medical_history) {
        free(p->name);
        free(p->medical_history);
        return false;
    }

    db->size++;
    return true;
}

// Updates a patient's medical history
bool update_medical_history(PatientDatabase* db, int id, const char* new_history) {
    if (!db || !new_history) return false;

    for (size_t i = 0; i < db->size; i++) {
        if (db->patients[i].id == id) {
            size_t new_size = strlen(new_history) + 1;
            if (new_size > db->patients[i].history_size) {
                char* resized = (char*)realloc(db->patients[i].medical_history, new_size);
                if (!resized) return false;
                db->patients[i].medical_history = resized;
                db->patients[i].history_size = new_size;
            }
            strcpy(db->patients[i].medical_history, new_history);
            return true;
        }
    }
    return false;
}

// Removes a patient by ID
bool remove_patient(PatientDatabase* db, int id) {
    if (!db) return false;

    for (size_t i = 0; i < db->size; i++) {
        if (db->patients[i].id == id) {
            free(db->patients[i].name);
            free(db->patients[i].medical_history);

            // Shift remaining patients
            memmove(&db->patients[i], &db->patients[i + 1], 
                   (db->size - i - 1) * sizeof(Patient));

            db->size--;
            return true;
        }
    }
    return false;
}

// Prints all patients
void print_database(const PatientDatabase* db) {
    if (!db) return;

    printf("\n=== Patient Database (%zu/%d) ===\n", db->size, MAX_PATIENTS);
    for (size_t i = 0; i < db->size; i++) {
        printf("ID: %d\nName: %s\nMedical History: %s\n\n",
               db->patients[i].id,
               db->patients[i].name,
               db->patients[i].medical_history);
    }
}

// Frees all allocated memory
void free_database(PatientDatabase* db) {
    if (!db) return;

    for (size_t i = 0; i < db->size; i++) {
        free(db->patients[i].name);
        free(db->patients[i].medical_history);
    }

    free(db->patients);
    free(db);
}

int main() {
    PatientDatabase* db = create_database(2);  // Start with small capacity
    if (!db) return EXIT_FAILURE;

    int choice;
    char name[100], history[500];
    int id;

    do {
        printf("\n=== Patient Records System ===\n");
        printf("1. Add Patient (Max %d)\n", MAX_PATIENTS);
        printf("2. Update Medical History\n");
        printf("3. Remove Patient\n");
        printf("4. View All Patients\n");
        printf("0. Exit\n");
        printf("> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (db->size >= MAX_PATIENTS) {
                    printf("Database full! Cannot add more patients.\n");
                    break;
                }
                printf("Enter patient name: ");
                scanf(" %99[^\n]", name);
                printf("Enter patient ID: ");
                scanf("%d", &id);
                printf("Enter medical history: ");
                scanf(" %499[^\n]", history);

                if (!add_patient(db, name, id, history)) {
                    printf("Failed to add patient!\n");
                }
                break;

            case 2:
                printf("Enter patient ID to update: ");
                scanf("%d", &id);
                printf("Enter new medical history: ");
                scanf(" %499[^\n]", history);

                if (!update_medical_history(db, id, history)) {
                    printf("Update failed! Patient not found.\n");
                }
                break;

            case 3:
                printf("Enter patient ID to remove: ");
                scanf("%d", &id);

                if (!remove_patient(db, id)) {
                    printf("Patient not found!\n");
                }
                break;

            case 4:
                print_database(db);
                break;

            case 0:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 0);

    free_database(db);
    return EXIT_SUCCESS;
}
