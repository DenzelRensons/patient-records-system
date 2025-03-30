# Patient Record Management System

![C Programming](https://img.shields.io/badge/language-C-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A comprehensive Patient Record Management System implemented in C with dynamic memory management and discharge tracking.

## Features

- **Dynamic Patient Management**
  - Add new patients with memory allocation
  - Update patient records (expandable medical history)
  - Mark patients as discharged
  - Remove discharged patients (with memory cleanup)

- **Data Persistence**
  - Automatic loading of records on startup
  - Manual saving to binary file
  - Handles variable-length data fields

- **Robust Memory Management**
  - Proper allocation/deallocation using malloc/free
  - Memory resizing with realloc
  - Comprehensive error checking

## Getting Started

### Prerequisites

- GCC compiler
- Linux/Unix environment (developed on Kali Linux)

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/DenzelRensons/patient-record-system.git
   cd patient-record-system
   ```

2. Compile the program:
   ```bash
   gcc patient_system.c -o patient_system
   ```

3. Run the executable:
   ```bash
   ./patient_system
   ```

## Usage

```
=== Hospital Patient Management System ===

Main Menu:
1. Add new patient
2. Update patient record
3. Remove discharged patients
4. View patient details
5. View all active patients
6. View all patients (including discharged)
7. Save data to file
8. Exit

Enter your choice (1-8): 
```

## Key Memory Management Features

This system implements core concepts from "Pointers and Memory Management":

- Dynamic memory allocation (`malloc`, `calloc`, `realloc`)
- Proper memory deallocation (`free`)
- Pointer arithmetic for array management
- Memory resizing for growing data structures
- Comprehensive error checking for memory operations

## File Structure

- `patient_system.c` - Main source code
- `patient_records.dat` - Database file (created after first save)
- `README.md` - This documentation

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

Based on concepts from "POINTERS AND MEMORY MANAGEMENT" presentation:
- Dynamic memory allocation techniques
- Proper memory management practices
- Pointer arithmetic implementations
