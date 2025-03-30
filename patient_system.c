#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define MAX_PATIENTS 100
#define INITIAL_HISTORY_SIZE 100
#define DATA_FILE "patient_records.dat"

typedef struct {
    char* name;            // Dynamically allocated
    int id;
    int age;
    char gender;           // 'M', 'F', or 'O'
    char* medical_history; // Dynamically allocated
    size_t history_size;   // Track allocated size
    time_t admission_date;
    time_t discharge_date;
    bool is_discharged;
} Patient;

Patient* patients[MAX_PATIENTS] = {NULL};
int patient_count = 0;

// Utility functions
void clear_input_buffer() {
    while (getchar() != '\n');
}

void print_header(const char* title) {
    printf("\n\033[1;36m=== %s ===\033[0m\n", title);
}

void print_success(const char* message) {
    printf("\033[1;32m%s\033[0m\n", message);
}

void print_error(const char* message) {
    printf("\033[1;31m%s\033[0m\n", message);
}

void print_warning(const char* message) {
    printf("\033[1;33m%s\033[0m\n", message);
}

char* format_time(time_t raw_time) {
    if (raw_time == 0) return "Not discharged";
    char* buffer = malloc(20);
    if (buffer) {
        strftime(buffer, 20, "%Y-%m-%d %H:%M", localtime(&raw_time));
    }
    return buffer;
}

// Patient management functions
Patient* find_patient(int id, bool include_discharged) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i] && patients[i]->id == id && 
            (include_discharged || !patients[i]->is_discharged)) {
            return patients[i];
        }
    }
    return NULL;
}

void display_patient(const Patient* p) {
    if (!p) {
        print_error("Patient not found.");
        return;
    }
    
    print_header("PATIENT DETAILS");
    printf("\033[1mID:\033[0m %d\n", p->id);
    printf("\033[1mName:\033[0m %s\n", p->name);
    printf("\033[1mAge:\033[0m %d\n", p->age);
    printf("\033[1mGender:\033[0m %c\n", p->gender);
    
    char* adm_date = format_time(p->admission_date);
    printf("\033[1mAdmission Date:\033[0m %s\n", adm_date);
    free(adm_date);
    
    char* dis_date = format_time(p->discharge_date);
    printf("\033[1mDischarge Date:\033[0m %s\n", dis_date);
    free(dis_date);
    
    printf("\033[1mStatus:\033[0m %s\n", p->is_discharged ? "DISCHARGED" : "ACTIVE");
    printf("\033[1mMedical History (%zu bytes allocated):\033[0m\n%s\n", 
           p->history_size, p->medical_history);
    printf("----------------------------\n");
}

void display_all_patients(bool show_discharged) {
    int count = 0;
    print_header(show_discharged ? "ALL PATIENTS (INCLUDING DISCHARGED)" : "ACTIVE PATIENTS");
    
    for (int i = 0; i < patient_count; i++) {
        if (patients[i] && (show_discharged || !patients[i]->is_discharged)) {
            display_patient(patients[i]);
            count++;
        }
    }
    
    if (count == 0) {
        print_warning("No patients found matching the criteria.");
    } else {
        printf("Total displayed: %d\n", count);
    }
}

Patient* create_patient() {
    Patient* new_patient = malloc(sizeof(Patient));
    if (!new_patient) {
        print_error("Failed to allocate memory for new patient.");
        return NULL;
    }
    
    // Initialize all fields
    new_patient->name = NULL;
    new_patient->medical_history = NULL;
    new_patient->history_size = 0;
    new_patient->admission_date = time(NULL);
    new_patient->discharge_date = 0;
    new_patient->is_discharged = false;
    
    return new_patient;
}

void free_patient(Patient* p) {
    if (p) {
        free(p->name);
        free(p->medical_history);
        free(p);
    }
}

bool resize_history(Patient* p, size_t new_size) {
    char* new_history = realloc(p->medical_history, new_size);
    if (!new_history && new_size > 0) {
        print_error("Failed to resize medical history.");
        return false;
    }
    p->medical_history = new_history;
    p->history_size = new_size;
    return true;
}

bool add_to_history(Patient* p, const char* new_info) {
    if (!p || !new_info) return false;
    
    size_t current_len = p->medical_history ? strlen(p->medical_history) : 0;
    size_t new_len = strlen(new_info);
    size_t needed_size = current_len + new_len + 3; // +3 for "; " and null terminator
    
    // First allocation if needed
    if (!p->medical_history) {
        if (!resize_history(p, needed_size)) return false;
        strcpy(p->medical_history, new_info);
        return true;
    }
    
    // Resize if needed
    if (needed_size > p->history_size) {
        if (!resize_history(p, needed_size * 2)) { // Double the size
            return false;
        }
    }
    
    // Append new information
    if (current_len > 0) {
        strcat(p->medical_history, "; ");
    }
    strcat(p->medical_history, new_info);
    
    return true;
}

void add_patient() {
    if (patient_count >= MAX_PATIENTS) {
        print_error("Hospital at maximum capacity. Cannot add more patients.");
        return;
    }
    
    Patient* new_patient = create_patient();
    if (!new_patient) return;
    
    print_header("ADD NEW PATIENT");
    
    // Get ID
    while (1) {
        printf("Enter patient ID (must be unique and positive): ");
        int id;
        if (scanf("%d", &id) == 1 && id > 0) {
            clear_input_buffer();
            if (find_patient(id, true)) {
                print_error("Patient with this ID already exists.");
                continue;
            }
            new_patient->id = id;
            break;
        }
        print_error("Invalid ID input.");
        clear_input_buffer();
    }
    
    // Get Name
    printf("Enter patient name: ");
    char name_buffer[100];
    fgets(name_buffer, sizeof(name_buffer), stdin);
    name_buffer[strcspn(name_buffer, "\n")] = '\0';
    new_patient->name = strdup(name_buffer);
    if (!new_patient->name) {
        print_error("Failed to allocate memory for name.");
        free_patient(new_patient);
        return;
    }
    
    // Get Age
    while (1) {
        printf("Enter patient age (1-120): ");
        if (scanf("%d", &new_patient->age) == 1 && 
            new_patient->age > 0 && new_patient->age <= 120) {
            clear_input_buffer();
            break;
        }
        print_error("Invalid age input.");
        clear_input_buffer();
    }
    
    // Get Gender
    while (1) {
        printf("Enter gender (M/F/O): ");
        new_patient->gender = toupper(getchar());
        clear_input_buffer();
        if (new_patient->gender == 'M' || 
            new_patient->gender == 'F' || 
            new_patient->gender == 'O') {
            break;
        }
        print_error("Invalid gender. Please enter M, F, or O.");
    }
    
    // Get Medical History
    printf("Enter initial medical history (press Enter to skip): ");
    char history_buffer[INITIAL_HISTORY_SIZE];
    fgets(history_buffer, sizeof(history_buffer), stdin);
    history_buffer[strcspn(history_buffer, "\n")] = '\0';
    
    if (strlen(history_buffer) > 0) {
        if (!add_to_history(new_patient, history_buffer)) {
            print_error("Failed to add medical history.");
            free_patient(new_patient);
            return;
        }
    } else {
        // Initialize with empty history
        if (!resize_history(new_patient, INITIAL_HISTORY_SIZE)) {
            free_patient(new_patient);
            return;
        }
        strcpy(new_patient->medical_history, "No medical history recorded");
    }
    
    // Add to patients array
    patients[patient_count++] = new_patient;
    print_success("Patient added successfully!");
    display_patient(new_patient);
}

void update_patient_record() {
    print_header("UPDATE PATIENT RECORD");
    if (patient_count == 0) {
        print_warning("No patients in the system.");
        return;
    }
    
    printf("Enter patient ID to update: ");
    int id;
    if (scanf("%d", &id) != 1) {
        print_error("Invalid ID input.");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    Patient* patient = find_patient(id, false);
    if (!patient) {
        print_error("Active patient with this ID not found.");
        return;
    }
    
    display_patient(patient);
    
    printf("\nWhat would you like to update?\n");
    printf("1. Medical history\n");
    printf("2. Mark as discharged\n");
    printf("Enter your choice: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        print_error("Invalid choice.");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    switch (choice) {
        case 1: {
            printf("Enter additional medical history: ");
            char new_history[INITIAL_HISTORY_SIZE];
            fgets(new_history, sizeof(new_history), stdin);
            new_history[strcspn(new_history, "\n")] = '\0';
            
            if (strlen(new_history) > 0) {
                if (add_to_history(patient, new_history)) {
                    print_success("Medical history updated successfully!");
                    display_patient(patient);
                } else {
                    print_error("Failed to update medical history.");
                }
            } else {
                print_warning("No changes made.");
            }
            break;
        }
        case 2: {
            printf("Are you sure you want to mark this patient as discharged? (y/n): ");
            char confirm = tolower(getchar());
            clear_input_buffer();
            
            if (confirm == 'y') {
                patient->is_discharged = true;
                patient->discharge_date = time(NULL);
                print_success("Patient marked as discharged.");
                display_patient(patient);
            } else {
                print_warning("Operation canceled.");
            }
            break;
        }
        default:
            print_error("Invalid choice.");
    }
}

void remove_discharged_patients() {
    print_header("REMOVE DISCHARGED PATIENTS");
    int removed_count = 0;
    
    for (int i = 0; i < patient_count; i++) {
        if (patients[i] && patients[i]->is_discharged) {
            printf("\nRemoving discharged patient:\n");
            display_patient(patients[i]);
            
            free_patient(patients[i]);
            patients[i] = NULL;
            removed_count++;
            
            // Shift remaining patients to fill the gap
            for (int j = i; j < patient_count - 1; j++) {
                patients[j] = patients[j + 1];
            }
            patients[patient_count - 1] = NULL;
            patient_count--;
            i--; // Recheck this index as it now has a new patient
        }
    }
    
    if (removed_count == 0) {
        print_warning("No discharged patients found to remove.");
    } else {
        print_success("Successfully removed discharged patients.");
        printf("Total removed: %d\n", removed_count);
    }
}

void save_to_file() {
    FILE* file = fopen(DATA_FILE, "wb");
    if (!file) {
        print_error("Could not open file for writing.");
        return;
    }
    
    // Write patient count
    fwrite(&patient_count, sizeof(int), 1, file);
    
    // Write each patient
    for (int i = 0; i < patient_count; i++) {
        if (patients[i]) {
            // Write fixed-size data
            fwrite(&patients[i]->id, sizeof(int), 1, file);
            fwrite(&patients[i]->age, sizeof(int), 1, file);
            fwrite(&patients[i]->gender, sizeof(char), 1, file);
            fwrite(&patients[i]->admission_date, sizeof(time_t), 1, file);
            fwrite(&patients[i]->discharge_date, sizeof(time_t), 1, file);
            fwrite(&patients[i]->is_discharged, sizeof(bool), 1, file);
            
            // Write name (length + string)
            size_t name_len = strlen(patients[i]->name) + 1;
            fwrite(&name_len, sizeof(size_t), 1, file);
            fwrite(patients[i]->name, sizeof(char), name_len, file);
            
            // Write medical history (length + string)
            size_t history_len = strlen(patients[i]->medical_history) + 1;
            fwrite(&history_len, sizeof(size_t), 1, file);
            fwrite(patients[i]->medical_history, sizeof(char), history_len, file);
            fwrite(&patients[i]->history_size, sizeof(size_t), 1, file);
        }
    }
    
    fclose(file);
    print_success("Patient data saved successfully.");
}

void load_from_file() {
    FILE* file = fopen(DATA_FILE, "rb");
    if (!file) {
        print_warning("No existing data file found. Starting with empty database.");
        return;
    }
    
    // Read patient count
    int saved_count;
    fread(&saved_count, sizeof(int), 1, file);
    
    for (int i = 0; i < saved_count && patient_count < MAX_PATIENTS; i++) {
        Patient* p = create_patient();
        if (!p) continue;
        
        // Read fixed-size data
        fread(&p->id, sizeof(int), 1, file);
        fread(&p->age, sizeof(int), 1, file);
        fread(&p->gender, sizeof(char), 1, file);
        fread(&p->admission_date, sizeof(time_t), 1, file);
        fread(&p->discharge_date, sizeof(time_t), 1, file);
        fread(&p->is_discharged, sizeof(bool), 1, file);
        
        // Read name
        size_t name_len;
        fread(&name_len, sizeof(size_t), 1, file);
        p->name = malloc(name_len);
        if (p->name) {
            fread(p->name, sizeof(char), name_len, file);
        } else {
            print_error("Failed to allocate memory for patient name.");
            free_patient(p);
            continue;
        }
        
        // Read medical history
        size_t history_len;
        fread(&history_len, sizeof(size_t), 1, file);
        p->medical_history = malloc(history_len);
        if (p->medical_history) {
            fread(p->medical_history, sizeof(char), history_len, file);
            fread(&p->history_size, sizeof(size_t), 1, file);
        } else {
            print_error("Failed to allocate memory for medical history.");
            free_patient(p);
            continue;
        }
        
        patients[patient_count++] = p;
    }
    
    fclose(file);
    printf("\nLoaded %d patient records from file.\n", patient_count);
}

void cleanup() {
    for (int i = 0; i < patient_count; i++) {
        free_patient(patients[i]);
    }
}

void show_menu() {
    print_header("HOSPITAL PATIENT RECORDS SYSTEM");
    printf("\033[1m1.\033[0m Add new patient\n");
    printf("\033[1m2.\033[0m Update patient record\n");
    printf("\033[1m3.\033[0m Remove discharged patients (free memory)\n");
    printf("\033[1m4.\033[0m View patient details\n");
    printf("\033[1m5.\033[0m View all active patients\n");
    printf("\033[1m6.\033[0m View all patients (including discharged)\n");
    printf("\033[1m7.\033[0m Save data to file\n");
    printf("\033[1m8.\033[0m Exit\n");
    printf("\033[1mEnter your choice (1-8): \033[0m");
}

int main() {
    printf("\033[1;36m=== Hospital Patient Management System ===\033[0m\n");
    load_from_file();
    
    while (1) {
        show_menu();
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            print_error("Invalid input. Please enter a number.");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: add_patient(); break;
            case 2: update_patient_record(); break;
            case 3: remove_discharged_patients(); break;
            case 4: {
                printf("Enter patient ID to view: ");
                int id;
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    display_patient(find_patient(id, true));
                } else {
                    print_error("Invalid ID input.");
                    clear_input_buffer();
                }
                break;
            }
            case 5: display_all_patients(false); break;
            case 6: display_all_patients(true); break;
            case 7: save_to_file(); break;
            case 8: 
                cleanup();
                print_success("Exiting system. All memory freed.");
                return 0;
            default:
                print_error("Invalid choice. Please try again.");
        }
        
        printf("\nPress Enter to continue...");
        clear_input_buffer();
    }
}
