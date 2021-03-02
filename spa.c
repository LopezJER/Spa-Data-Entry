/*

Spa Employee System

This program organizes employee and appointment information through a linked list data structure. Users can add, edit, view, and delete one or all employees and appointments of a spa via a menu interface. Data regarding employees are stored in alphabetical order while appointments are stored in ascending order. Furthermore, users are notified if they attempt to create appointments that conflict with preexisting ones, i.e. within 30 minutes of old appointments. Users can save employee and appointment information via text files. 

@Author Jose Enrique R. Lopez
@Date Created 10-12-19


*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define ACTIVE 1
#define EXITED 0

typedef struct app_node{
	int id;
	struct tm schedule;
	struct app_node * next;
} Appointment;

typedef struct name{
	char last[20];
	char first[20];
} Name;

typedef struct date{
	int month;
	int day;
	int year;
} Date;

typedef struct emp_node{
	int empNum;
	Name name;
	int age;
	char position [30];
	struct tm dateHired;
    Appointment * app;                                                                                                                                                                                             
	struct emp_node * next;
} Employee;

//utilities functions
void printBanner();
int showMainMenu();
int showEmpMenu();
int showAppMenu();
int enterEmpNum();
int enterAppId();
struct tm inputDate (struct tm timestamp);
struct tm inputTime (struct tm timestamp);
void showEmpDetails (Employee * emp);
void showApps (Employee * emp);

//save/load functions
void saveEmployees (Employee * head, FILE * fp);
void saveAppointments (Employee * head, FILE * fp);

//primary functions
Employee * loadEmployees (Employee * head, FILE * fp);
void loadAppointments (Appointment ** ptr, FILE ** fl);
Employee * addEmployee(Employee * head, Employee * emp);
Employee * createEmployee();
void editEmployee(Employee * head);
Employee * delEmployee(Employee * head);
void viewAllEmps(Employee * head);
void viewEmpByNum(Employee * head);
Employee * findEmp (Employee * head, int empNum);
Employee * findBookedEmp (Employee * head, int appId);
void viewByPosition(Employee * head);
void viewEmployee(Employee * head);
Appointment * addAppointment(Appointment * head, Appointment * newApp);
Employee * editAppointment(Employee * head, Appointment * appHead, int id);
Employee * delAppointment(Employee * head, int id);
void showAppDetails (Appointment * app);
Appointment * delAppByNum(Appointment * head, int * success);

int maxGlobalID = 0;

int main (void){
	FILE * fp, * fl;
	int status = ACTIVE;
	Employee * head = NULL;
	head = loadEmployees(head, fp);

	
	while (status == ACTIVE){
		Employee * emp = NULL;
		switch(showMainMenu()){
			case 1: switch(showEmpMenu()){
						case 1: head = addEmployee(head, emp);	break;
						case 2: editEmployee(head);			break;
						case 3: head = delEmployee(head);	break;
						case 4: viewEmployee(head);			break;
						case 0: break;
						default: printf("Please pick a valid option.");		
					} break;
					
			case 2: switch(showAppMenu()){	
					int choice;
					Appointment * newApp;
					int id;
					case 1:
						printf("Book a 30-minute appointment with one of our lovely staff!");
						newApp = NULL;
						viewAllEmps(head);
						choice = enterEmpNum(head);
						emp = findEmp(head, choice);
						if (emp!=NULL){
							Appointment * appHead = emp->app;
							emp->app = addAppointment(appHead, newApp);  
						} else{
							printf("Employee does not exist!");
						}
					break;
					case 2:
					id = enterAppId();
					emp  = findBookedEmp(head, id);
					if (emp!= NULL){
						printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
						printf("\nAppointment Details: \n");
						Appointment * appHead = emp->app;  
						head = editAppointment (head, appHead, id);
 					} else{
						printf("Appointment does not exist!");
					}
					
					break;
					case 3:
					id = enterAppId();
					head = delAppointment(head, id);
					break;
					case 0:
						break;
					default:
						printf("Please pick a valid option.");	
				}
				break;
			case 0: saveEmployees(head, fp); saveAppointments(head,fp);	status = EXITED;	break;
			default: printf("\nPlease pick a valid option.\n");		break;
		}
	}
	return 0;
}

void saveEmployees (Employee * head, FILE * fp){
	Employee * temp = NULL;
	temp = head;
	fp = fopen("employees.txt", "w");
	while (temp!=NULL){
		fprintf(fp, "-----------EMPLOYEE INFO-----------\n");
		fprintf(fp, "%s", temp->name.last);
		fprintf(fp, "%s", temp->name.first);
		fprintf(fp, "%d\n", temp->empNum);
		fprintf(fp, "%d\n", temp->age);
		fprintf(fp, "%s\n", temp->position);
		char date[12];
		strftime(date, sizeof(date), "%x", &temp->dateHired);
		fprintf(fp, "%s\n", date);		
		temp = temp->next;
	}
	fclose(fp);
}

void saveAppointments (Employee * head, FILE * fp){
	Employee * temp = NULL;
	temp = head;
	fp = fopen("appointments.txt", "w");
	while (temp!=NULL){
		Appointment * appTemp = NULL;
		appTemp = temp->app;
		while(appTemp!=NULL){
			char appDate[20];
			strftime(appDate, sizeof(appDate), "%x|%H:%M", &appTemp->schedule);
			fprintf (fp, "%s|%d\n", appDate, appTemp->id);
			appTemp = appTemp->next;
		}	
		fprintf(fp, "---END---\n");
		temp = temp->next;
	}

	fclose(fp);
}

void loadAppointments(Appointment **ptr, FILE ** fl){
	
	Appointment * head = NULL, *temp = NULL;
	Appointment *app =NULL;
	do{
		time_t now; time (&now);
		struct tm timestamp = *localtime(&now);
		struct tm schedule = *localtime(&now);
		int appMonth, appYear;
		
		char line[100], *p;
		
		fgets (line, 100, *fl);
		
		if(feof(*fl)) printf("END!!\n");
		
		if(strcmp(line, "---END---\n")==0){
			if (head!=NULL){
			}
			*ptr = head;
			
			break;
		}
		
		app = (Appointment*)malloc(sizeof(Appointment));
		
		p=strtok (line, "|");
		sscanf(p, "%d/%d/%d", &appMonth, &schedule.tm_mday, &appYear);
		schedule.tm_mon = appMonth-1;
		schedule.tm_year =(appYear + 2000) -1900;
		
		p=strtok (NULL, "|");
		sscanf(p, "%d:%d", &schedule.tm_hour, &schedule.tm_min);
		p=strtok (NULL, "|"); 
		sscanf(p, "%d", &app->id);
		
		app->schedule = schedule;
		
		char date[30];
		strftime(date, sizeof(date), "%x", &app->schedule);
		printf("SCHEDULE: %s\n", date);
		app->next = NULL;
		head = addAppointment(head, app);
		
		if (head!=NULL){
				//printf("HEAD->ID %d\n", (*(head))->id);
			}
		printf("Inserted new data!\n");
		
	} while (1);

	
}

Employee * loadEmployees (Employee * head, FILE * fp){
	time_t now; time (&now);
	struct tm timestamp = *localtime(&now);
	struct tm schedule = *localtime(&now);
	int month, year;
	fp = fopen("employees.txt", "r");
	int idCounter = 0;
	if (fp != NULL){
		char firstLine[100];
		while (fgets(firstLine, 100, fp) != NULL){
			Employee * newEmp = (Employee *) malloc(sizeof(Employee));
			char id[5], age[5], date[30];
			fgets(newEmp->name.last, 50, fp);
			fgets(newEmp->name.first, 50, fp);
			fgets(id, 15, fp);
			fgets(age, 15, fp);
			newEmp->age = atoi (age);
			newEmp->empNum = atoi(id);
			fgets(newEmp->position, 30, fp);
			int len = strlen(newEmp->position);
			if( newEmp->position[len-1] == '\n' )
				newEmp->position[len-1] = 0;
			fgets(date, 30, fp);
			sscanf(date,"%d/%d/%d", &month, &timestamp.tm_mday, &year);
			timestamp.tm_mon = month - 1;
			if (year < 50){
			timestamp.tm_year = (year+2000)-1900; 
			} else if (year > 50 && year < 100){
				//timestamp.tm_year = year + 1900;
			}
			newEmp->dateHired = timestamp;
			newEmp->app = NULL;
			newEmp->next = NULL;
			head = addEmployee (head, newEmp);
			//showEmpDetails(newEmp);
			idCounter++;
		}
	} else{}
	fclose(fp);
	maxGlobalID = idCounter;
	return head;
}

void printBanner(){
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n SPA MOMENTS ONLINE PORTAL \n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
int showMainMenu(){
	printBanner();
	printf("\nChoose a category:\n\n");
	printf("[1] Employees\n");
	printf("[2] Appointments\n");
	printf("\n[0] Exit\n\n");
	
	int choice;
	printf("Enter choice: ");
	scanf("%d", &choice);
	return choice;
}

int showEmpMenu(){
	printBanner();
	printf("\nSelect choice: \n\n");
	printf("[1] Add Employee\n");
	printf("[2] Edit Employee\n");
	printf("[3] Delete Employee\n");
	printf("[4] View Employee\n");
	printf("\n [0] Back to main menu\n");
	int choice;
	printf("\nEnter choice: ");
	scanf("%d", &choice);
	return choice;
}

int showAppMenu(){
	printBanner();
	printf("\nSelect choice: \n\n");
	printf("[1] Add Appointment\n");
	printf("[2] Edit Appointment\n");
	printf("[3] Delete Appointment\n");
	printf("\n [0] Back to main menu\n");
	int choice;
	printf("\nEnter choice: ");
	scanf("%d", &choice);
	return choice;
}


void generateId (Employee * emp){
	int EmpNum = maxGlobalID; 
	int choice;
	EmpNum++;
	emp->empNum = EmpNum;
	maxGlobalID++;
}
void enterName (Employee * emp){
	
	char char_buffer;
	scanf("%c", &char_buffer);
	printf("Last name: ");
	fgets(emp->name.last, 50, stdin);
	printf("First name: ");
	fgets(emp->name.first, 50, stdin);
}
void enterAge (Employee * emp){
	int status = ACTIVE;
	do{
		printf("Age: ");
		if (scanf("%d", &emp->age)!=1){
			printf("NOTE: Invalid age. \n");
			scanf("%*s");
		} else{
			status=EXITED;
		}
	} while (status==ACTIVE);
	
}
void enterPos (Employee * emp){
	char char_buffer;
	int choice;
	scanf("%c", &char_buffer);
	printf("Available Positions: ");
	const char positions[8][30] = {"AESTHETICIAN", "HAIR STYLIST", "MASSAGE THERAPIST", "NAIL TECHNICIAN", "SALON SERVICES ATTENDANT", "SPA ATTENDANT", "SPA MANAGEMENT", "SPA RECEPTIONIST"};
	
	int status = ACTIVE;
	do{
		printf("\n\t\t\tPOSITIONS\n");
		printf("[1]Aesthetician  \t[5]Salon Services Attendant\n[2]Hair Stylist \t[6]Spa Attendant\n[3]Massage Therapist \t[7]Spa Management \n[4]Nail technician \t[8]Spa Receptionist\n");
		printf("\nEnter valid position: ");
		if ((scanf("%d", &choice)!=1)){
			printf("NOTE: Invalid choice. \n");
			scanf("%*s");
		} else{
			if (choice < 1 || choice > 8){
				printf("NOTE: Invalid choice. \n");
			} else{
			status=EXITED;
			}
		}
	} while (status==ACTIVE);
	
	choice--;
	strcpy(emp->position, positions[choice]);
	
}

void enterDateHir (Employee * emp){
	char date[12];
	struct tm dateHired;
	printf("DATE HIRED: ");
	dateHired = inputDate(emp->dateHired);
	emp->dateHired=dateHired;

}

int enterEmpNum(){
	
	int choice;
	int status = ACTIVE;
	do{
		printf("\n>>Enter ID of employee: ");
		if (scanf("%d", &choice)!=1){
			printf("NOTE: Invalid ID No.");
			scanf("%*s");
		} else{
			status=EXITED;
		}
	} while (status==ACTIVE);
	return choice;
}

int enterAppId(){

	int choice;
	int status = ACTIVE;
	do{
		printf("\n>>Enter appointment ID: ");
		if (scanf("%d", &choice)!=1){
			printf("NOTE: Invalid ID No.");
			scanf("%*s");
		} else{
			status=EXITED;
		}
	} while (status==ACTIVE);
	return choice;
}

int confirmChoice(){
		while (ACTIVE){
			char choice [5];
			scanf("%s", choice);
			if (strcmp(choice, "Y")==0){
				return ACTIVE;
			} else if (strcmp(choice, "N")==0){
				return EXITED;
			} else{
				printf("Please enter Y/N only: ");
			}
		}
	
}

struct tm inputDate (struct tm timestamp){
	int status=ACTIVE;
	time_t now;
	time (&now);
	
	timestamp = *localtime(&now);
	do{
		time_t app_time_t;
		struct tm app_tm;
		char date[20];
		char myApp[20];

		
		printf("Enter date (mm/dd/yy): ");
		scanf("%s", date);
		int len = strlen(date);
		int month, year;
		sscanf(date,"%d/%d/%d", &month, &timestamp.tm_mday, &year);
		timestamp.tm_mon = month -1;
		
			if (year < 50){
			timestamp.tm_year = (year+2000)-1900; 
			} else if (year > 50 && year < 100){
				timestamp.tm_year = year + 1900;
			}
		
		if (mktime(&timestamp)==-1){
			printf("Incorrect format. Please try again.\n");
		} else{
			app_time_t = mktime(&timestamp);
			app_tm = *localtime(&app_time_t);
			strftime(myApp, sizeof(myApp), "%x", &app_tm);
			if (strcmp(myApp, date) != 0){
				printf("Incorrect format. Please try again.");
			} else{
				status = EXITED;
			}
		} 
	}while(status==ACTIVE);
	
	return timestamp;
	
}

struct tm inputTime (struct tm timestamp){
	int status=ACTIVE;
	do{
	time_t app_time_t;
	struct tm app_tm;
	char time[10];
	char myApp[20];
	
	printf("Enter time of appointment (hh:mm) (24-hr format): ");
	scanf("%s", time);
	sscanf(time, "%d:%d", &timestamp.tm_hour, &timestamp.tm_min);
	
		if (mktime(&timestamp)==-1){
			printf("Incorrect format. Please try again.\n");
		} else {
			timestamp.tm_sec = 0; //DO NOT REMOVE!
			app_time_t = mktime(&timestamp);
			app_tm = *localtime(&app_time_t);
			strftime(myApp, sizeof(myApp), "%H:%M", &app_tm);
			if (strcmp(myApp, time) != 0){
				printf("Incorrect format. Please try again.");
			}else{
				status=EXITED;
			}
	
		}
	}while (status == ACTIVE);
	
	return timestamp;
}

Employee * createEmployee(){
	Employee * newEmp = (Employee *) malloc (sizeof(Employee));
	
	printf("ENTER NEW EMPLOYEE DETAILS: \n");

	generateId(newEmp);
	enterName(newEmp);
	enterAge(newEmp);
	enterPos(newEmp);
	enterDateHir(newEmp);
	
	printf("\nAssigned ID No. %d to Mr./Ms. %s", newEmp->empNum, newEmp->name.last);
	newEmp->app= NULL;
	newEmp->next = NULL;
	
	printf("\n***********************\nNew Recruit Summary\n***********************\n");
	showEmpDetails(newEmp);
	printf("************************\n");
	
	return newEmp;
	
	
}

int compareNames(Employee * emp1, Employee * emp2){
	
	if (strcmp(emp1->name.last, emp2->name.last) != 0){
		return strcmp(emp1->name.last, emp2->name.last);
	} else {
		return strcmp(emp1->name.first, emp2->name.first);
	}

}


Employee * addEmployee(Employee * head, Employee * newEmp){
	if (newEmp == NULL){
		newEmp = createEmployee();
	}
	
	Employee * temp = NULL;
	temp = head;

	
	if (head == NULL|| compareNames(newEmp, head) <0){ //insert at head
		newEmp->next = head;
		head = newEmp;
	}
	else{
		while (temp->next!=NULL && compareNames(newEmp, temp->next) >0){
			temp = temp->next;
		}
		if (temp->next == NULL){ //add at tail
			newEmp->next = NULL;
			temp->next = newEmp;
		} else{ //add at middle
			newEmp -> next = temp->next;
			temp->next = newEmp;
		}


	}
	

	return head;
}
	
void editEmployee(Employee * head){
	int empNum = enterEmpNum();
	Employee * emp = findEmp(head, empNum);
	if (emp != NULL){
		int status = ACTIVE;
		int choice;
		while (status == ACTIVE){
			
			printf("\n[1]Name\n[2]Age\n[3]Position\n[0]Exit\nSelect entry to be edited: ");
			scanf("%d", &choice);
			switch(choice){
				case 1:
					enterName(emp);
					break;
				case 2:
					enterAge(emp);
					break;
				case 3:
					enterPos(emp);
					break;
				case 0:
					status = EXITED;
					break;
				default:
					printf("Please enter valid option");
					break;
			}
		}
	} else{
		printf("Employee does not exist.");
	}
}


Employee * delEmpByNum (Employee * head){
	viewAllEmps(head);
	printf("\n>>Delete employee info...");
	int empNum = enterEmpNum();
	Employee * temp;
	temp = head;
	
	if (temp!=NULL && temp->empNum == empNum){
		printf("Are you sure you want to delete the following employee data? (Y/N): ");
		showEmpDetails(temp);
		
		int confirm = confirmChoice();
		if (confirm == ACTIVE){
				head = temp-> next;
				free (temp);
				printf("\n>>Successfully deleted.\n");
				return head; 

		} else{
			printf("\n>>...\n");
			return head;
		}
	} else{
		while(temp!=NULL && temp->next!=NULL && temp->next->empNum != empNum){
			temp = temp -> next;
		}

		if (temp == NULL || temp->next==NULL){
			printf("Employee does not exist.");
			return head;
		}else{
			printf("Are you sure you want to delete the following employee data? (Y/N): ");
			showEmpDetails(temp->next);
		
			int confirm = confirmChoice();
			if (confirm == ACTIVE){
					Employee * del = temp -> next;
					temp->next = temp->next->next;
					free(del);
					printf("\n>>Successfully deleted.\n");
					return head; 
			} else{
				printf("\n>>...\n");
				return head;
			}
		}	
	}
}

Employee * delAllEmps (Employee * head){
	Employee * temp;
	temp = head;
	
	if (temp == NULL){
		return head;
	} else{
		head = temp-> next;
		free (temp);
		return delAllEmps(head);
	}
}

Employee * delEmployee(Employee * head){
	while (1){
	printf("\n[1] Delete one employee\n[2] Delete all employees\n");
	printf("Enter option: ");
	int choice;
	scanf ("%d", &choice);
	if (choice == 1){
		head = delEmpByNum (head);
		break;
	} else if (choice == 2){
		printf("Are you sure you want to delete all employee data? Cannot be undone (Y/N): ");
		int confirm = confirmChoice();
		if (confirm == ACTIVE){
			printf(">>Deleting all employees");
			head = delAllEmps (head);
		} else{
			printf(">>...");
		}
		break;
	} else{
		printf("Please pick a valid option.");
	}
}
}


void viewAllEmps(Employee * head){
	printBanner();
	Employee * temp = NULL;
	temp = head;
	char full_name[30];
	printf("\n-------------------------\nFULL LIST OF EMPLOYEES\n-------------------------\n");
	while (temp!=NULL){
		printf("Surname: %s", temp->name.last);
		printf("Given Name: %s", temp->name.first);
		printf("Employee Number: %d\n", temp->empNum);
		printf("Age: %d\n", temp->age);
		printf("Position: %s\n", temp->position);
		char date[30];
		strftime(date, sizeof(date), "%x", &temp->dateHired);
		printf("Date Hired: %s\n\n", date);
		//printf("TEMP->NEXT->NAME.last = %s", temp->next->name.last); 
		temp = temp->next;
	}
}

Employee * findEmp (Employee * head, int empNum){
	Employee * emp = NULL;
	emp = head;

	while (emp!=NULL && emp->empNum!=empNum){
		emp = emp->next;
	}
	
	return emp;
	
}

Employee * findBookedEmp (Employee * head, int appId){
	Employee * emp = NULL;
	emp = head;
	Appointment * app = NULL;
	while (emp!=NULL){
		app = emp->app;
		while (app!=NULL && app->id!=appId){
			app=app->next;

		}
		if (app!=NULL){
			return emp;
		}
		emp = emp->next;
	}
	return emp;
}

void showEmpDetails (Employee * emp){
	if (emp!=NULL){
		printf("\nSurname: %s", emp->name.last);
		printf("Given Name: %s", emp->name.first);
		printf("\nEmployee Number: %d\n", emp->empNum);
		printf("Age: %d\n", emp->age);
		printf("Position: %s\n", emp->position);
		char date[12];
		strftime(date, sizeof(date), "%x", &emp->dateHired);
		printf("Date Hired : %s\n", date);
	}
	else{
		printf("Employee does not exist!");
	}
}


void viewEmpByNum(Employee * head){
	Employee * emp;
	int empNum = enterEmpNum();
	emp = findEmp (head, empNum);
	showEmpDetails (emp);
	showApps (emp);
}

void viewByPosition(Employee * head){
	Employee * temp = NULL;
	temp = head;
	
	printf("Enter position: \n");
	printf("[1]Aesthetician  \t[5]Salon Services Attendant\n[2]Hair Stylist \t[6]Spa Attendant\n[3]Massage Therapist \t[7]Spa Management \n[4]Nail technician \t[8]Spa Receptionist\n");
	char positions[8][30] = {"AESTHETICIAN", "HAIR STYLIST", "MASSAGE THERAPIST", "NAIL TECHNICIAN", "SALON SERVICES ATTENDANT"," SPA ATTENDANT", "SPA MANAGEMENT", "SPA RECEPTIONIST"};
	int choice;
	scanf ("%d", &choice);
	choice--;
	char position [30]; 
	strcpy(position, positions[choice]);
	printf("Viewing all: %s\n", position);

	while (temp!=NULL){
		if (strcmp(temp->position, position)== 0){
			
			printf("\nEmployee Number: %d\n", temp->empNum);
			printf("Surname: %s", temp->name.first);
			printf("First Name: %s", temp->name.last);
			printf("Age: %d\n", temp->age);
			printf("Position: %s\n", temp->position);
			printf("Date Hired : %d/%d/%d\n", temp->dateHired.tm_mon,temp->dateHired.tm_mday, temp->dateHired.tm_year);
		}
		temp = temp->next;
	}

}

void viewEmployee(Employee * head){
	while (ACTIVE){
		printf("\n[1] View one employee\n[2] View all employees\n[3] View by position\n");
		printf("Enter option: ");
		int choice;
		scanf ("%d", &choice);
		if (choice == 1){
			//viewAllEmps(head);
			viewEmpByNum(head);
			break;
		} else if (choice == 2){
			viewAllEmps(head);
			break;
		} else if (choice == 3){
			viewByPosition(head);
			break; 
		} else{
			printf("Please pick a valid option.");
		}
	}
}

int generateAppId(){
		static int id = 1; 
		int genId = id;
		id++;
		
		return genId;
		
}
Appointment * createAppointment (){

	int id = generateAppId();
	Appointment * newApp = (Appointment * ) malloc (sizeof(Appointment)); 
	newApp->id = id;
	newApp->next = NULL;
	
	int status = ACTIVE;
	
	struct tm timestamp;
	
	timestamp = inputDate(timestamp);
	timestamp = inputTime(timestamp);

	newApp->schedule = timestamp;

	return newApp;
	
	
}


void showApps(Employee * emp){
	if(emp!=NULL){
		Appointment * temp = emp->app;		
		while (temp!=NULL){
			char appString [30];
			strftime(appString, sizeof(appString), "%x at %I:%M%p", &temp->schedule);
			printf("ID No.: %d | Schedule: %s\n", temp->id, appString);
			temp = temp->next;
		}
	}
}

int checkAppointment (Appointment * head, Appointment * newApp){
		Appointment * temp = NULL;
		char appString[30];
		temp = head;
		
		printf("Hello world!");
		
		if (difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) >-1800 && difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) <1800){
				printf("Proposed appointment conflicts with existing appointment: ID No. %d", head->id);
				return 0;
		}else{
			while (temp->next!=NULL && difftime(mktime(&newApp->schedule), mktime(&temp->schedule))> 1800){
				temp = temp->next;
			} if (difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) > -1800 && difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) <1800){
				printf("Proposed appointment conflicts with existing appointment: ID No. %d", temp->id);
				return 0;
			}
		return 1;
	}
}

Appointment * addAppointment(Appointment * head, Appointment * app){
		static int isLoadingFile = 0;
		Appointment * newApp;
		if (head!=NULL){
		printf("head->id %d\n", head->id);
		}
		if (app == NULL){
			newApp = createAppointment();
		} else{
			Appointment newAppInfo = *app;
			newApp = &newAppInfo;
		}

		Appointment * temp = NULL;
		char appString[30];
		temp = head;
		if (head == NULL || difftime(mktime(&newApp->schedule), mktime(&head->schedule)) <= -1800){
			newApp->next = head;
			head = newApp;

		} else if (difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) >-1800 && difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) <1800){
				printf("Proposed appointment conflicts with existing appointment: ID No. %d\n", head->id);
				newApp->id = -1;
				return head;
		}else{
			while (temp->next!=NULL && difftime(mktime(&newApp->schedule), mktime(&temp->schedule))> 1800){
				temp = temp->next;
			} if (difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) > -1800 && difftime(mktime(&newApp->schedule), mktime(&temp->schedule)) <1800){
				printf("Proposed appointment conflicts with existing appointment: ID No. %d\n", temp->id);
				newApp->id = -1;
				return head;
			}
			 else if (temp->next == NULL){ //add at tail	
				newApp->next = NULL;
				temp->next = newApp;
			}else{ //add at middle
				newApp -> next = temp->next;
				temp->next = newApp;
			}

	}
	if (isLoadingFile == 0){
		//ON SUCCESS SCHEDULING OF APPOINTMENT
		strftime(appString, sizeof(appString), "%x at %I:%M%p", &newApp->schedule);
		printf("You have scheduled an appointment on %s with Appointment ID no. %d\n", appString, newApp->id);
	} else{
		isLoadingFile = 0;
	}
	return head;

}

Appointment * findAppointment (Appointment * appHead, int id){
	Appointment * app = NULL;
	app = appHead;
	while (app!=NULL && app->id!=id){
		app = app->next;
	}
	
	return app;
	
}
Employee * editAppointment(Employee * head, Appointment * appHead, int id){
	Appointment * app = findAppointment (appHead, id);
	if (app!=NULL){
		int choice, status;
		showAppDetails(app);
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		do{
			printf("Select entry you wish to update.\n");
			printf("[1]Date\n[2]Time\n[3]Employee Assigned\n\t[0]Exit\n");
			printf("\nEnter valid option: ");
			if ((scanf("%d", &choice)!=1)){
				printf("NOTE: Invalid choice. \n");
				scanf("%*s");
			} else{
				time_t now;
				time (&now);
				struct tm newSched = *localtime(&now);
				Appointment oldApp, newApp;
				Employee * emp = NULL;

				switch (choice){
					case 1:
						oldApp = *app;
						newApp = *app;
						head = delAppointment (head, app->id);
						newSched = inputDate(newSched);
						newSched.tm_hour = app->schedule.tm_hour;
						newSched.tm_min = app->schedule.tm_min;
						newApp.schedule = newSched;
						newApp.id = generateAppId();
						appHead = addAppointment(appHead, &newApp);
						if (newApp.id!=-1){
							//head = delAppointment (head, app->id);
							printf("\nDate successfully updated\n");
						} else{
							appHead = addAppointment (appHead, &oldApp);
							printf("Please schedule at another time.");
							}
						break;
					case 2:
					//MODIFY ID! Remember!
						oldApp = *app;
						newApp = *app;
						head = delAppointment (head, app->id);
						newSched = inputTime(newSched);
						newSched.tm_mday = app->schedule.tm_mday;
						newSched.tm_mon = app->schedule.tm_mon;
						newSched.tm_year = app->schedule.tm_year;
						newApp.schedule = newSched;
						newApp.id = generateAppId();
						appHead = addAppointment(appHead, &newApp);
						if (newApp.id!=-1){
							//head = delAppointment (head, app->id);
							printf("\nTime successfully updated\n");
						} else{
							appHead = addAppointment (appHead, &oldApp);
							printf("Please schedule at another time.");
						}
						break;
					case 3:
						viewAllEmps(head);
						printf("Enter new employee: \n");
						int choice; 
						choice = enterEmpNum(head);
						emp = findEmp(head, choice);
						if (emp!=NULL){
							Appointment * appHead = emp->app;
							Appointment newApp = *app;
							appHead = addAppointment(appHead, &newApp);
							if (newApp.id!=-1){
								head = delAppointment (head, app->id);
								printf("\nEmployee assigned successfully updated\n");
							} else{
								printf("Please schedule at another time.");
							}
						} else{
							printf("Employee does not exist!");
						}
						
						break;
					case 0:
						status=EXITED;
						break;
					default:
						printf("NOTE: Invalid option. \n");
				}
			}
		} while (status==ACTIVE);
	}
	return head;
}

Employee * delAppointment(Employee * head, int id){
	Employee * emp = head;
	int success = 0;
	
	while (emp != NULL && success == 0){
		Appointment * appHead = emp->app;
		Appointment * temp;
		
		temp = appHead;
	
		if (temp!=NULL && temp->id == id){
			printf("Please confirm the requested action on the following appointment (Y/N): ");
			showAppDetails(temp);
			
			int confirm = confirmChoice();
			if (confirm == ACTIVE){
					emp->app = NULL;
					free (temp);
					printf("\n>>Confirmed.\n");
					success = ACTIVE;
					break;

			} else{
				printf("\n>>...\n");
				success = ACTIVE;
				break;
			}
		} else{
			while(temp!=NULL && temp->next!=NULL && temp->next->id != id){
				temp = temp -> next;
			}
			
			if (temp == NULL || temp->next==NULL){
			}else{
				printf("Please confirm the requested action on the following appointment (Y/N): ");
				showAppDetails(temp->next);
			
				int confirm = confirmChoice();
				if (confirm == ACTIVE){
						Appointment * del = temp->next;
						temp->next = temp->next->next;
						free(del);
						printf("\n>>Confirmed.\n");
						success = ACTIVE; 
						break;
				} else{
					printf("\n>>...\n");
					success = ACTIVE;
					break;
				}
	
			}
		}
		emp = emp -> next;
	}
		
	if (success == 0){
		printf("Appointment does not exist!");
	}
	return head;
}
		

void showAppDetails (Appointment * app){
	char appString [30];
	strftime(appString, sizeof(appString), "%x at %I:%M%p", &app->schedule);
	printf("ID No.: %d | Schedule: %s\n", app->id, appString);
	
}

