
/*
** clientMenu.c
** Author: Vladislav Makarov
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define _APPT_PLACE 65 // 64 Characters Max
#define _APPT_DESCRIPTION 129 // 128 Characters Max

FILE *outfile;
FILE *infile;

struct contact

{
    char username[20], password[20], email[30], phoneNumber[18];

} user, possible_user, empty_user;

typedef struct {

    struct tm start;
    struct tm end;
    char place[_APPT_PLACE];
    char description[_APPT_DESCRIPTION];

} appointment;

char currentUsername[20] = "not authenticated";
char filename[40];
char * fileExtension = ".txt";
char * appointmentExtension = "_appointment.txt";
bool userAuthenticated = false;
int selection;
appointment currentAppointment;
appointment emptyAppointment;

void displayMainMenu();
void displayAppointmentsMenu();
void authorizeExistingUser();
void addNewUser();
void removeUser();
void modifyUserInformation();
void saveInformation();
void clearScreen();
void flush();
appointment newAppointment();
void removeAppointments();
void loadAppointments();
void showAppointments();
void modifyAppointment();
void saveAppointment(appointment appt);


int main(int argc, char *argv[])
{
    clearScreen();
    printf("Hello and Welcome to Personal Calendar App!\n");
    displayMainMenu();
}

void displayMainMenu() {
    flush();
    printf("\n\n\tMAIN MENU\n\n");
    printf("[current user: %s]\n", currentUsername);

    printf("0. Authenticate Existing User\n");
    printf("1. Add User\n");
    printf("2. Remove User\n");
    printf("3. Modify User Information\n");
    printf("4. View Appointments Menu\n");
    printf("5. Exit\n\n");
    printf("Selection: ");

    scanf("%d", &selection);

    switch(selection) {

       case 0  :
          if (userAuthenticated) {
              flush();
              printf("You are already Authenticated as: %s\n", currentUsername);
              printf("Would you like to Sign Out? (1 = yes, 0 = no)   : ");
              int areYouSure = getchar();
              areYouSure -= '0';

              if (areYouSure == 1) {
                  printf("\nYou are now Signed Out. Returning to Main Menu...\n");
                  userAuthenticated = false;
                  strncpy(currentUsername, "not authenticated", 20);
                  displayMainMenu();
              } else {
                  printf("Sign Out canceled. Returning to Main Menu...\n");
                  displayMainMenu();
              } 
              displayMainMenu();
          } else {
              authorizeExistingUser();
          }
          break;
        
       case 1  :
          addNewUser();
          break;

       case 2  :
          if (userAuthenticated) {
              removeUser();
          } else {
              printf("To perform a User Delete, you must first Sign In.\nPlease Sign In and try again.\n");
              displayMainMenu();
          }
          break;

       case 3  :
          if (userAuthenticated) {
              modifyUserInformation();
          } else {
              printf("To modify User Account Info, you must first Sign In.\nPlease Sign In and try again.\n");
              displayMainMenu();
          }
          break;

       case 4 :
          if (userAuthenticated) {
            printf("Now showing the Appointments Menu...\n");
            displayAppointmentsMenu();
          } else {
            printf("To display Appointments Menu, you must first Sign In.\nPlease Sign In and try again.\n");
            displayMainMenu();
          }
          break;

       case 5  :
          //saveInformation();
          printf("Now exiting... Goodbye!\n\n");
          exit(0);
          break;
      
       default :
          printf("Invalid choice. Please try again, or enter '5' to Exit.\n");
          displayMainMenu();
          break;
    }
}

void authorizeExistingUser() {
    userAuthenticated = false;

    while (!userAuthenticated) {
        char Username[20];
        char *Password;
        
        printf("\nPlease enter your username: ");
        scanf("%s", &Username); 

        //printf("Please enter your password: ");
        //scanf("%s", &Password); 
        Password = getpass("Please enter your password: ");

        strncpy(filename, Username, sizeof(Username));
        strcat(filename, fileExtension);
        
        infile = fopen(filename, "r");
        if(infile == NULL)
        {
            printf("\nA user with entered Username does not exist. Please try again.\n");
        } else {
        
            int result = fscanf(infile, "%[^;];%[^;];%[^;];%[^;]", &possible_user.username, &possible_user.password, &possible_user.email, &possible_user.phoneNumber);

            // For testing
            //printf("Entered username: %s , Compared username: %s\n", &Username, &possible_user.username);
            //printf("Entered password: %s , Compared password: %s\n", Password, &possible_user.password);

            if (result != NULL) {
                if (strcmp(&possible_user.username, &Username) == 0 && strcmp(&possible_user.password, Password) == 0) {
                    user = possible_user;
                    //printf("\nLOADED DETAILS: user: %s ; password: %s ; email: %s ; phone: %s\n", &possible_user.username, &possible_user.password, &possible_user.email, &possible_user.phoneNumber);
                    printf("\n*** AUTHORIZATION SUCCESS ***\n");
                    printf("Welcome, %s!\n", &user.username);
                    strncpy(currentUsername, &user.username, 20);
                    userAuthenticated = true;
                }  else {
                    printf("\n*** AUTHORIZATION FAILURE ***\n");
                    printf("The Username and/or Password you entered is invalid. Please try again.\n");
                    userAuthenticated = false;
                }
            } else {
                printf("\n*** UNABLE TO VERIFY CREDENTIALS ***\n");
            }
        }
        
        possible_user = empty_user;
        fclose(infile);
        displayMainMenu();
    }
}

void addNewUser() {
    flush();
    char Username[20];
    printf("To return to Main Menu, enter 0 for Username\n\nUsername: ");
    scanf("%s",&Username);

    if(strcmp(Username,"0") == 0) {
        displayMainMenu();
     }
    
    strncpy(filename, Username, sizeof(Username));
    strcat(filename, fileExtension);
    
    if( access( filename, F_OK ) == -1 ) {

        //printf("Password: ");
        //scanf("%s",&user.password);
        bool passwordAccepted = false;
        while (!passwordAccepted) {
            char *Password = getpass("Please enter your password: ");
            if (strlen(Password) > 8 && strlen(Password) < 16) {
                strncpy(&user.password, Password, 20);
                passwordAccepted = true;
            } else {
                printf("Password should be longer than 8 and shorter than 16 characters.\nTry again...\n");
            }
        }

        printf("Phone Number: ");
        scanf("%s",&user.phoneNumber);

        printf("Email address: ");
        scanf("%s", &user.email);

        strncpy(&user.username, Username, 20);
        strncpy(currentUsername, &user.username, 20);
        userAuthenticated = true;
        
        printf("\n*** NEW USER ADDED SUCCESSFULLY ***\n");
        printf("You are now authenticated as: %s\n", &user.username);
        printf("Entered Phone Number: %s\n", &user.phoneNumber);
        printf("Entered Email Address: %s\n", &user.email);
        printf("\nNow returning to Main Menu...\n");
        saveInformation();
    } else {
        printf("\n*** USER WITH NAME '%s' ALREADY EXISTS ***\n", Username);
        printf("Please try different Username.\n");         
    }
    displayMainMenu();
}

void removeUser() {
    flush();
    printf("You are currently signed in as: %s\n", currentUsername);
    printf("Are you sure you want to Delete this User? (1 = yes, 0 = no)   : ");
    int areYouSure = getchar();
    areYouSure -= '0';

    if (areYouSure == 1) {
        strncpy(filename, currentUsername, sizeof(currentUsername));
        strcat(filename, fileExtension);

        //printf("user file to delete: %s\n", filename);
        printf("\n*** REMOVING USER: %s *** \n", &currentUsername);

        int result = remove(filename);

        if(result == 0) {
           printf("User removed successfully. Returning to Main Menu...\n");
           userAuthenticated = false;
           strncpy(currentUsername, "not authenticated", 20);
           user = empty_user;
        } else {
           printf("Error: unable to delete the user. Please try again.\n");
        }
    } else {
        printf("User Delete request canceled. Returning to Main Menu...\n");
    }
    
    displayMainMenu();
}

void modifyUserInformation() {
    flush();
    printf("Please enter new details for User: %s\n\nNew Name: ", currentUsername);
    strncpy(&user.username, currentUsername, 20);

    //printf("Password: ");
    //scanf("%s",&user.password);
    bool passwordAccepted = false;
    while (!passwordAccepted) {
        char *Password = getpass("New Password: ");
        if (strlen(Password) > 8 && strlen(Password) < 16) {
            strncpy(&user.password, Password, 20);
            passwordAccepted = true;
        } else {
            printf("New password should be longer than 8 and shorter than 16 characters.\nTry again...\n");
        }
    }

    printf("New Phone Number: ");
    scanf("%s",&user.phoneNumber);

    printf("New Email address: ");
    scanf("%s", &user.email);

    printf("\n*** USER INFORMATION UPDATED SUCCESSFULLY ***\n");
    printf("You are authenticated as: %s\n", &user.username);
    printf("New Phone Number: %s\n", &user.phoneNumber);
    printf("New Email Address: %s\n", &user.email);
  
    printf("\nNow returning to Main Menu...\n");

    strncpy(currentUsername, &user.username, 20);

    userAuthenticated = true;
    saveInformation();
    displayMainMenu();
}

void saveInformation() {
    if (userAuthenticated) {
        strncpy(filename, currentUsername, sizeof(currentUsername));
        strcat(filename, fileExtension);
        //printf("%s", filename);
        
        outfile = fopen (filename, "w");
        if (outfile == NULL)
        {
            fprintf(stderr, "\nError saving User Data to file %s\n", filename);
        } else {

            // Write user information to file
            // Data written in following order: username, password, email, phoneNumber
            // Delimiter used - ";"
            int result = fprintf(outfile, "%s;%s;%s;%s", &user.username, &user.password, &user.email, &user.phoneNumber);

            if(result != 0) {
                printf("\nUser Information saved successfully!\n");
            } else {
                printf("\nError saving User Information\n");
            }
            fclose(outfile);
        }
    } else {
        printf("\nYou are not Signed in, so there is no Information to save.\nTo Save Information you need to either Sign In or Add a New User.\n");
    }
}

void displayAppointmentsMenu() {
    flush();
    printf("\n\n\tAPPOINTMENTS MENU\n\n");
    printf("[current user: %s]\n", currentUsername);

    printf("0. Add Appointment\n");
    printf("1. Remove Appointments\n");
    printf("2. Show My Appointments\n");
    printf("3. Update Existing Appointment\n");
    printf("4. Go back to Main Menu\n");
    printf("5. Exit\n\n");
    printf("Selection: ");

    scanf("%d", &selection);

    switch(selection) {

       case 0  :
          currentAppointment = newAppointment();
          displayAppointmentsMenu();
          break;
        
       case 1  :
          removeAppointments();
          break;

       case 2  :
          showAppointments();
          break;
          
       case 3  :
          modifyAppointment();
          break;

       case 4 :
          printf("Now showing the Main Menu...\n");
          displayMainMenu();
          break;

       case 5  :
          printf("Now exiting... Goodbye!\n\n");
          exit(0);
          break;
      
       default :
          printf("Invalid choice. Please try again, or enter '5' to Exit.\n");
          displayAppointmentsMenu();
          break;
    }
}

void clearScreen() {
    for (int i = 0; i < 20; ++i) {
        printf("\n");
    }
}

void flush() {
    fseek(stdin,0,SEEK_END);
}

appointment newAppointment() {
  appointment appt;
  
  printf("You will now be asked to enter Appointment Details for your Appointment.\n\n");
  
  printf("\nPlease enter appointment Start Date (example input: 04-03-2018)\nDate Input: ");
  scanf("%d-%d-%d", &appt.start.tm_mday, &appt.start.tm_mon, &appt.start.tm_year);
  printf("\nPlease enter appointment Start Time (example input: 15:30)\n\Time Input: ");
  scanf("%d:%d", &appt.start.tm_hour, &appt.start.tm_min);
  printf("Your entered: %d:%d %d-%d-%d\n" , appt.start.tm_hour, appt.start.tm_min, appt.start.tm_mday ,appt.start.tm_mon , appt.start.tm_year);
  
  printf("\nPlease enter appointment End Date (example input: 04-03-2018)\nDate Input: ");
  scanf("%d-%d-%d", &appt.end.tm_mday, &appt.end.tm_mon, &appt.end.tm_year);
  printf("\nPlease enter appointment End Time (example input: 17:30)\n\Time Input: ");
  scanf("%d:%d", &appt.end.tm_hour, &appt.end.tm_min);
  printf("Your entered: %d:%d %d-%d-%d\n" , appt.end.tm_hour, appt.end.tm_min, appt.end.tm_mday ,appt.end.tm_mon , appt.end.tm_year);
  
  printf("\nPlease enter appointment Place/Location (up to 64 characters; example input: 8th floor Conference Room, Lawrence Street Center)\n\Input: ");
  flush();
  scanf("%[^\n]s", &appt.place);
  printf("Your entered: %s\n" , appt.place);
  
  printf("\nPlease enter appointment Contents/Description (up to 128 characters; example input: Meet to discuss future semester classes)\n\Input: ");
  flush();
  scanf("%[^\n]s", &appt.description);
  printf("Your entered: %s\n" , appt.description); 
  
  printf("\n*** NEW APPOINTMENT ADDED SUCCESSFULLY ***\n");
  printf("Now returning to Appointments Menu...\n");
  
  saveAppointment(appt);
  return appt;
}

void removeAppointments() {
    strncpy(filename, currentUsername, sizeof(currentUsername));
    strcat(filename, appointmentExtension);
            
    if( access( filename, F_OK ) != -1 ) {
        flush();
        printf("Are you sure you want to delete all User Appointments? (1 = yes, 0 = no)   : ");
        int areYouSure = getchar();
        areYouSure -= '0';

        if (areYouSure == 1) {
            printf("\n*** REMOVING APPOINTMENTS FOR USER: %s *** \n", &currentUsername);

            int result = remove(filename);

            if(result == 0) {
                currentAppointment = emptyAppointment;
                printf("Appointments Removed Successfully.\n");
            } else {
                printf("No Existing Appointments found to delete.\n");
            }
        } else {
            printf("\nAppointments Deletion canceled.\n");
        }

    } else {
        printf("\n*** NO EXISTING APPOINTMENTS FOUND TO DELETE ***\n");
    }

    printf("Now returning to Appointments Menu...\n");
    displayAppointmentsMenu();
}

void loadAppointments() {
    strncpy(filename, currentUsername, sizeof(currentUsername));
    strcat(filename, appointmentExtension);
        
    infile = fopen(filename, "r");
    if(infile != NULL)
    {
        appointment appt;
        int result = fscanf(infile, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%[^;];%[^;]", 
                        &appt.start.tm_mday, &appt.start.tm_mon, &appt.start.tm_year, &appt.start.tm_hour, &appt.start.tm_min,                   
                        &appt.end.tm_mday, &appt.end.tm_mon, &appt.end.tm_year, &appt.end.tm_hour, &appt.end.tm_min,
                        &appt.place, &appt.description);
        if (result != NULL) {
            currentAppointment = appt;

        }
    }
}

void showAppointments() {
    strncpy(filename, currentUsername, sizeof(currentUsername));
    strcat(filename, appointmentExtension);
    //printf("\nfilename: %s\n", filename);
        
        infile = fopen(filename, "r");
        if(infile == NULL)
        {
            printf("\n*** NO EXISTING APPOINTMENTS FOUND TO SHOW ***\n");
        } else {
            appointment appt;
            int result = fscanf(infile, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%[^;];%[^;]", 
                            &appt.start.tm_mday, &appt.start.tm_mon, &appt.start.tm_year, &appt.start.tm_hour, &appt.start.tm_min,                   
                            &appt.end.tm_mday, &appt.end.tm_mon, &appt.end.tm_year, &appt.end.tm_hour, &appt.end.tm_min,
                            &appt.place, &appt.description);
            // For testing
            //printf("Entered username: %s , Compared username: %s\n", &Username, &possible_user.username);
            //printf("Entered password: %s , Compared password: %s\n", Password, &possible_user.password);

            if (result != NULL) {
                currentAppointment = appt;
                printf("\n*** SHOWING LAST APPOINTMENT DETAILS ***\n");
                printf("Appointment Start Date: %d:%d %d-%d-%d\n" , currentAppointment.start.tm_hour, currentAppointment.start.tm_min, currentAppointment.start.tm_mday ,currentAppointment.start.tm_mon , currentAppointment.start.tm_year);
                printf("Appointment End Date %d:%d %d-%d-%d\n" , currentAppointment.end.tm_hour, currentAppointment.end.tm_min, currentAppointment.end.tm_mday ,currentAppointment.end.tm_mon , currentAppointment.end.tm_year);
                printf("Place/Location: %s\n" , currentAppointment.place);
                printf("Contents/Description: %s\n\n" , currentAppointment.description);
            } else {
                printf("\n*** NO EXISTING APPOINTMENTS FOUND TO DISPLAY ***\n");
            }
        }
  
    printf("Now returning to Appointments Menu...\n");
    displayAppointmentsMenu();
}

void modifyAppointment() {
    loadAppointments();
    strncpy(filename, currentUsername, sizeof(currentUsername));
    strcat(filename, appointmentExtension);
            
    if( access( filename, F_OK ) != -1 ) {
        flush();
        printf("You will now be asked to enter new details for your existing Appointment...\n\n");
 
        printf("\nAppointment Start Date/Time (current value): %d:%d %d-%d-%d\n" , currentAppointment.start.tm_hour, currentAppointment.start.tm_min, currentAppointment.start.tm_mday ,currentAppointment.start.tm_mon , currentAppointment.start.tm_year);
        printf("Please enter new appointment Start Date (example input: 04-03-2018)\nDate Input: ");
        scanf("%d-%d-%d", &currentAppointment.start.tm_mday, &currentAppointment.start.tm_mon, &currentAppointment.start.tm_year);
        printf("\nPlease enter new appointment Start Time (example input: 15:30)\n\Time Input: ");
        scanf("%d:%d", &currentAppointment.start.tm_hour, &currentAppointment.start.tm_min);

        printf("\nAppointment End Date/Time (current value): %d:%d %d-%d-%d\n" , currentAppointment.end.tm_hour, currentAppointment.end.tm_min, currentAppointment.end.tm_mday ,currentAppointment.end.tm_mon , currentAppointment.end.tm_year);
        printf("Please enter new appointment End Date (example input: 04-03-2018)\nDate Input: ");
        scanf("%d-%d-%d", &currentAppointment.end.tm_mday, &currentAppointment.end.tm_mon, &currentAppointment.end.tm_year);
        printf("\nPlease enter new appointment End Time (example input: 17:30)\n\Time Input: ");
        scanf("%d:%d", &currentAppointment.end.tm_hour, &currentAppointment.end.tm_min);
        
        printf("\nAppointment Place/Location (current value): %s\n" , currentAppointment.place);
        printf("Please enter appointment Place/Location (up to 64 characters; example input: 8th floor Conference Room, Lawrence Street Center)\n\Input: ");
        flush();
        scanf("%[^\n]s", &currentAppointment.place);

        printf("\nAppointment Contents/Description (current value): %s\n" , currentAppointment.description); 
        printf("Please enter appointment Contents/Description (up to 128 characters; example input: Meet to discuss future semester classes)\n\Input: ");
        flush();
        scanf("%[^\n]s", &currentAppointment.description);

        printf("\n*** APPOINTMENT INFORMATION UPDATED SUCCESSFULLY ***\n");
        printf("Now returning to Appointments Menu...\n");
        saveAppointment(currentAppointment);
    } else {
        printf("\n*** NO EXISTING APPOINTMENTS FOUND TO MODIFY ***\n");
        printf("Now returning to Appointments Menu...\n");
    } 
    displayAppointmentsMenu();
}

void saveAppointment(appointment appt) {
    if (userAuthenticated) {
        strncpy(filename, currentUsername, sizeof(currentUsername));
        strcat(filename, appointmentExtension);
        //printf("\nfilename: %s\n", filename);
        
        outfile = fopen (filename, "w");
        if (outfile == NULL)
        {
            fprintf(stderr, "\nError saving Appointment Data to file %s\n", filename);
        } else {

            // Write appointment information to file
            // Data written in following order: start date, end date, place, description;
            // Delimiter used - ";"
            int result = fprintf(outfile, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%s;%s",          
                                appt.start.tm_mday, appt.start.tm_mon, appt.start.tm_year, appt.start.tm_hour, appt.start.tm_min,                   
                                appt.end.tm_mday, appt.end.tm_mon, appt.end.tm_year, appt.end.tm_hour, appt.end.tm_min,
                                &appt.place, &appt.description);

            if(result != 0) {
                printf("\nAppointment Information saved successfully!\n");
            } else {
                printf("\nError saving Appointment Information\n");
            }
            fclose(outfile);
        }
    } else {
        printf("\nYou are not Signed in, so there is no Information to save.\nTo Save Information you need to either Sign In or Add a New User.\n");
    }
}
