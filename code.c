#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct courseProfile{
    char aYear[10];
    int sem;
    char code[15];
    int units;
    float grade;
} courseProfile;

typedef struct courseToUnit{
    char code[15];
    int units;
} courseToUnit;

// takes a course code and returns how many units that course has
int codeToUnits(char* code){
    int codeToUnits(char* cCode){
    FILE* data = fopen("courseCodeUnits.csv", "r");
    if(!data){
        printf("Error. File \"courseCodeUnits.csv\" not found.\n");
        exit(1);
    }
    char buffer[100];
    int temp;
    long pos;
    courseToUnit c;

    fseek(data,0, SEEK_SET);

    while(!feof(data)) {
        fgets(buffer,100,data);
        char *data = strtok(buffer, ",");
        if (data == NULL) break;
        strcpy(c.code, data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.units = atoi(data);

        temp = strcmp(cCode,c.code);

        if(temp==0){
            return c.units;
        }
    }
    fclose(data);
    return 3;
}

// prints the entire file
void printFile(FILE* fp){
    printf("A.Y. SEMESTER COURSE UNITS GRADE\n");
    fseek(fp, 0, SEEK_SET);
    char buffer[100];
    while(fgets(buffer,100,fp)!=NULL) {
        char *data = strtok(buffer, ",");
        while (data !=NULL){
            printf("%s ", data);
            data = strtok(NULL, ",");
        }
    }
}

// allows users to input a course and writes that into the file
void inputCourse(FILE* fp){
    courseProfile c;
    int ans;
    char buffer[100];

    fseek(fp, 0, SEEK_END);

    printf(
        "Before we begin, please be reminded of the applications strict input formats.\n"
        "Academic school years should be inputted in the format YYYY-YYYY.\n"
        "Semesters inputs should be a number between 1 to 3. 1 meaning the first semester,\n2 meaning the second semester, and 3 meaning the third/summer semester.\n"
        "Course codes should be inputed WITHOUT spaces and letters should ALL be uppercase.\nThe course codes should be the same as those used in the official study plan\ngiven by the OUR.\n\n"
        "Sample Input: 2024-2025 2 CMSC21\n\n"
    );

    while(1) {
        printf("\nEnter the academic school year, semester, and course you're taking:\n");
        scanf("%s %d %s", c.aYear, &c.sem, c.code);
        c.units = codeToUnits(c.code);
        c.grade = 0.00;
        printf("\nDo you wish to input a grade? Input 1 if yes, 0 if no. \n");
        scanf("%d", &ans);
        if(ans){
            printf("Enter the grade you recieved:\n");
            scanf("%f", &c.grade);
        }
        fprintf(fp,"%s,%d,%s,%d,%0.2f\n", c.aYear, c.sem, c.code, c.units, c.grade);
        printf("\nDo you wish to continue inputting courses? Input 1 if yes, 0 if no. \n");
        scanf("%d", &ans);
        if(!ans){
            break;
        }
    }
    fseek(fp, 0, SEEK_SET);
    printFile(fp);
}

// allows users to edit the file to add in the grades of classes taken in a specific semester of a specific acad year
void inputGrade(FILE* fp){
    courseProfile c;
    int ans;
    char buffer[100];
    int sems,count, temp;
    char ay[10];
    long pos;
    printf("Enter the academic school year and semester you wish to calculate the GWA for:\n");
    scanf("%s %d", ay, &sems);

    fseek(fp,0, SEEK_SET);

    while(!feof(fp)) {
        pos=ftell(fp);
        fgets(buffer,100,fp);
        char *data = strtok(buffer, ",");

        if (data == NULL) break;
        strcpy(c.aYear, data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.sem = atoi(data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        strcpy(c.code, data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.units = atoi(data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.grade = atof(data);

        temp = strcmp(ay,c.aYear);

        if(temp==0 && sems==c.sem && c.grade==0){
            printf("Do you wish to add the grade you recieved for the class %s? Input 1 if yes, 0 if no.\n",c.code);
            scanf("%d", &ans);
            if(ans){
                printf("Enter the grade you recieved:\n");
                scanf("%f", &c.grade);
                fseek(fp, pos,SEEK_SET);
                fprintf(fp,"%s,%d,%s,%d,%0.2f\n", c.aYear, c.sem, c.code, c.units, c.grade);
                fflush(fp);
            }
            count=1;
            printf("\nDo you wish to continue inputting grades? Input 1 if yes, 0 if no. \n");
            scanf("%d", &ans);
            if(!ans){
                break;
            }
        }
    }

    printf("There are no");
    if(count){
        printf(" more");
    }
    printf(" courses inputted that occur within the school year and semester you inputted.\n\n"
    "Here is the updated database.\n\n"
    );
    printFile(fp);
    return;

}

/* takes the inputted acad year and semester, scours the entire file for all the courses which were taken during the
provided time and computes for the gwa and returns the result
*/
float gwaCal (FILE* fp, char* ay, int sems){
    courseProfile c;
    float grades=0, gwa=0;
    char buffer[100];
    int temp, unit=0;
    long pos;
    int scCount=0;

    fseek(fp,0, SEEK_SET);
    while(!feof(fp)) {
        pos=ftell(fp);
        fgets(buffer,100,fp);

        char *data = strtok(buffer, ",");
        if (data == NULL) break;
        strcpy(c.aYear, data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.sem = atoi(data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        strcpy(c.code, data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.units = atoi(data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.grade = atof(data);

        temp = strcmp(ay,c.aYear);

        if(temp==0 && sems==c.sem){
            grades += (c.grade*c.units);
            unit+= c.units;
            scCount++;
        }
    }

    if(scCount){
        gwa = grades/unit;
        return gwa;
    }

    return 0;
}

// takes an academic year and semester from the user, calculates the GWA for that semester and prints the result
void calcuGWA(FILE* fp){
    courseProfile c;
    char  ay[10];
    int sems, unit=0;


    printf("Enter the academic school year and semester you wish to calculate the GWA for:\n");
    scanf("%s %d", ay, &sems);

    float gwa = gwaCal(fp,ay,sems);
    int check = gwa;

    printf("Your GWA in the ");
    switch(sems){
        case 1:
            printf("1st ");
            break;
        case 2:
            printf("2nd ");
            break;
        case 3:
            printf("3rd/Summer ");
            break;
    }
    printf("semester of the %s A.Y. is ", ay);
    switch(check){
        case 0:
            printf("unavailable.\nKindly check if the inputted academic school year and semester have courses with grades in the database.");
            break;
        default:
            printf("%0.2f.", gwa);
            break;
    }

};

void forecastGWA(FILE* fp){
    printf("WIP");
}


int main(void){
    FILE* fp = fopen("user.csv","r+");
    int choice;
    if(!fp){
        FILE* fp = fopen("user.csv","w+");
        return 0;
    }
    do {
        printf("HELLO USER!\n");
        printf("\nPlease Select an Option\n");
        printf("*****************************\n");
        printf("(1) Input Course\n");
        printf("(2) Input Grades\n");
        printf("(3) Calculate GWA\n");
        printf("(4) Forecast GWA\n");
        printf("(5) Print database\n");
        printf("(0) Exit\n");
        printf("*****************************\n");
        printf("ENTER CHOICE: ");
        scanf("%d", &choice);

        printf("\n*****************************\n\n");

        switch (choice) {
            case 1:
                inputCourse(fp);
                break;
            case 2:
                inputGrade(fp);
                break;
            case 3:
                calcuGWA(fp);
                break;
            case 4:
                break;
            case 5:
                printFile(fp);
                break;
            case 0:
                printf("Exiting program\n");
                break;
            default:
                printf("Invalid choice!\n");
                break;
        }

        printf("\n*****************************\n\n\n\n");

    } while (choice != 0);


   fclose(fp);
}
