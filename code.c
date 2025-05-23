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

// stores the range of grades the user predicts theyll get. for the forecast mode
typedef struct minmaxGrade{
    float worst;
    int numWorst;
    float best;
    int numBest;
} minmaxGrade;

// flushes the input stream
int flush_stdin() {
    char buffer[256];

    fgets(buffer, 256, stdin);
    if (buffer[0] == '\n')
        return 0;

    while (!strchr(buffer, '\n')) {
        fgets(buffer, 256, stdin);
    }

    return 1;
}

// takes a course code and returns how many units that course has
int codeToUnits(char* cCode){
    FILE* data = fopen("courseCodeUnits.csv", "r");
    if(!data){
        printf("Error. File \"courseCodeUnits.csv\" not found.\n");
        exit(1);
    }
    char buffer[30];
    courseToUnit c;

    fseek(data,0, SEEK_SET);

    // will read the entire database looking for the inputted course title
    while(!feof(data)) {
        fgets(buffer,30,data);
        char *data = strtok(buffer, ",");
        if (data == NULL) break;
        strcpy(c.code, data);

        data = strtok(NULL, ",");
        if (data == NULL) break;
        c.units = atoi(data);



        if(strcmp(cCode,c.code)==0){
            return c.units;
        }
    }
    fclose(data);
    //if the function cannot find the course title in the database it returns 0
    return 0;
}

// counts how many courses were entered in the inputted acad year and semester
int courseCounter(FILE* fp, char* ay, int sems){
    courseProfile c;
    char buffer[100];
    int count=0, temp;

    fseek(fp,0, SEEK_SET);

    while(!feof(fp)) {
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
            count++;
        }
    }
    return count;
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

// checks if the grade is valid
int isValidGrade(float grade){
    float temp = 4*grade;
    return (int) temp == temp;
}

// allows users to input a course and writes that into the file containing the database
void inputCourse(FILE* fp){
    courseProfile c;
    int ans, read, temp, temp2;
    char buffer[500];

    fseek(fp, 0, SEEK_END);

    printf(
        "Before we begin, please be reminded of the applications strict input formats.\n"
        "Academic school years should be inputted in the format YYYY-YYYY.\n"
        "Semesters inputs should be a number between 1 to 3. 1 meaning the first semester,\n2 meaning the second semester, and 3 meaning the third/summer semester.\n"
        "Course codes should be inputed WITHOUT spaces and letters should ALL be uppercase.\nThe course codes should be the same as those used in the official study plan\ngiven by the OUR.\n\n"
        "Sample Input: 2024-2025 2 CMSC21\n\n"
        "*****************************"
    );

    while(1) {
        for(;;){
            printf("\nEnter the academic school year, semester, and course you're taking:\n");
            while (fgets(buffer,sizeof(buffer),stdin) && buffer[0] == '\n');
            read = sscanf(buffer,"%s %d %s", c.aYear, &c.sem, c.code);

            if(read!=3){
                printf("Error. Incorrect format. Please try again and input what is requested in the proper format.\n\n");
                continue;
            }

            char *data = strtok(buffer, "-");
            temp = atoi(data);

            if(temp<1908){
                printf("Error. The inputted academic year was before UP was established. Please try again.\n\n");
                continue;
            }
            data = strtok(NULL, " ");
            temp2 = atoi(data);

            if((temp2-temp)!=1){
                printf("Error. The inputted years are more than 1 year apart. Please try again.\n\n");
                continue;
            }

            if(!(codeToUnits(c.code))){
                printf("Error. The inputted course code isn't in the database. Please try again.\n\n");
                continue;
            }

            break;
        }
        c.units = codeToUnits(c.code);
        c.grade = 0.00;

        for(;;){
            printf("\nDo you wish to input a grade? Input 1 if yes, 0 if no. \n");
            read = scanf("%d", &ans);
            if(flush_stdin() || read < 1 || (ans!=1 && ans!= 0)){
                printf("Error! Invalid input. Inputs must either be 1 for yes or 0 for no. Try again.\n\n");
                continue;
            }
            break;
        }

        if(ans){
            for(;;){
                printf("Enter the grade you recieved:\n");
                read=scanf("%f", &c.grade);

                if(flush_stdin() || read < 1){
                    printf("Error! Invalid input. Inputs must be a number between 1.00 and 5.00. Try again.\n\n");
                    continue;
                }
                if(c.grade>3.00 && c.grade<5.00){
                    printf("Error! Invalid input. A grade in the range (3.00,5.00) does not exist in the UP grading system.\nA grade of 4.0 does exist, however, it is a temporary classification.\n\n");
                    continue;
                }

                if((c.grade<1.00 || c.grade>5.00) || !(isValidGrade(c.grade))){
                    printf("Error! Invalid input. The UP grading system uses numbers in 0.25 increments. Try again\n\n");
                    continue;
                }
                break;
            }
        }
        fprintf(fp,"%s,%d,%s,%d,%0.2f\n", c.aYear, c.sem, c.code, c.units, c.grade);

        for(;;){
            printf("\nDo you wish to continue inputting courses? Input 1 if yes, 0 if no. \n");
            read = scanf("%d", &ans);
            if(flush_stdin() || read < 1 || (ans!=1 && ans!= 0)){
                printf("Error! Invalid input. Inputs must either be 1 for yes or 0 for no. Try again.\n\n");
                continue;
            }
            break;
        }
        if(!ans){
            break;
        }
    }
    printf(
        "\n*****************************\n"
        "Here is the updated database.\n\n"
    );
    fseek(fp, 0, SEEK_SET);
    printFile(fp);
}

// allows users to edit the file to add in the grades of classes taken in a specific semester of a specific acad year
void inputGrade(FILE* fp){
    courseProfile c;
    int ans,read;
    char buffer[100];
    int sems,count;
    char ay[10];
    long pos;

    for(;;){
        printf("Enter the academic school year and semester you wish to input grades for:\n");
        while (fgets(buffer,sizeof(buffer),stdin) && buffer[0] == '\n');
        read = sscanf(buffer,"%s %d", ay, &sems);

        if(read!=2){
            printf("\nError. Inputs must be in the format 20XX-20XX 2\nTry again.\n\n");
            continue;
        }
        if(courseCounter(fp,ay,sems)<1){
            printf("\nError. There are no courses in the inputted semester. Try again.\n\n");
            continue;
        }
        break;
    }

    fseek(fp,0, SEEK_SET);

    printf("\n*****************************\n");

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

        if(strcmp(ay,c.aYear)==0 && sems==c.sem && c.grade==0){
            for(;;){
                printf("\NDo you wish to add the grade you recieved for the class %s? Input 1 if yes, 0 if no.\n",c.code);
                read = scanf("%d", &ans);
                if(flush_stdin() || read < 1 || (ans!=1 && ans!= 0)){
                    printf("Error! Invalid input. Inputs must either be 1 for yes or 0 for no. Try again.\n\n");
                    continue;
                }
                break;
            }

            if(ans){
                for(;;){
                    printf("Enter the grade you recieved:\n");
                    read=scanf("%f", &c.grade);

                    if(flush_stdin() || read < 1){
                        printf("Error! Invalid input. Inputs must be a number between 1.00 and 5.00. Try again.\n\n");
                        continue;
                    }
                    if(c.grade>3.00 && c.grade<5.00){
                        printf("Error! Invalid input. A grade in the range (3.00,5.00) does not exist in the UP grading system.\nA grade of 4.0 does exist, however, it is a temporary classification.\n\n");
                        continue;
                    }

                    if((c.grade<1.00 || c.grade>5.00) || !(isValidGrade(c.grade))){
                        printf("Error! Invalid input. The UP grading system uses numbers in 0.25 increments. Try again\n\n");
                        continue;
                    }
                    break;
                }

                fseek(fp, pos,SEEK_SET);
                fprintf(fp,"%s,%d,%s,%d,%0.2f\n", c.aYear, c.sem, c.code, c.units, c.grade);
                fflush(fp);
            }
            count=1;

            for(;;){
                printf("\nDo you wish to continue inputting grades? Input 1 if yes, 0 if no. \n");
                read = scanf("%d", &ans);
                if(flush_stdin() || read < 1 || (ans!=1 && ans!= 0)){
                    printf("Error! Invalid input. Inputs must either be 1 for yes or 0 for no. Try again.\n");
                    continue;
                }
                break;
            }

            if(!ans){
                break;
            }
        }
    }

    printf("\n*****************************\n");

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
    int scCount=0;

    fseek(fp,0, SEEK_SET);
    while(!feof(fp)) {
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
    char  ay[10], buffer[500];
    int sems,read;

    for(;;){
        printf("Enter the academic school year and semester you wish to calculate the GWA for:\n");
        while (fgets(buffer,sizeof(buffer),stdin) && buffer[0] == '\n');
        read = sscanf(buffer,"%s %d", ay, &sems);

        if(read!=2){
            printf("\nError. Inputs must be in the format 20XX-20XX 2\nTry again.\n\n");
            continue;
        }
        if(courseCounter(fp,ay,sems)<1){
            printf("\nError. There are no courses in the inputted semester. Try again.\n\n");
            continue;
        }
        break;
    }

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
            printf("%.3f.\n", gwa);
            break;
    }

}

// does the necessary calculations to determine what grades the user must acquired given the set parameters
void forecaster (char* ay, int sems, float gwa, courseProfile *arr, int size,minmaxGrade *range){
    float target, tempGWA=0, bestgradeGWA, worstgradeGWA;
    int sumUnits=0, units=5, running_heavy=0, running_light=0, max=0,preset_units=0;
    int unit_count[6]={0};
    int nHeavy = range->numWorst, nLight = range->numBest;

    //counts how many classes have x amount of units
    for(int i=0; i<size;i++){
        if(arr[i].grade!=0){
            tempGWA=tempGWA + (arr[i].grade*arr[i].units);
            preset_units+=arr[i].units;
            continue;
        }
        sumUnits+=arr[i].units;
        unit_count[arr[i].units]++;
    }
    target = gwa*(sumUnits+preset_units);

    int running[6]={0};

    while(units>0 && nHeavy>0 && sumUnits>0){
        if (unit_count[units]==0){
            units--;
            continue;
        } else {
            unit_count[units]--;
            running[units]++;
            nHeavy--;
            running_heavy+=units;
            sumUnits-=running_heavy;
            max=units;
        }
    }
    units=1;
    int min=0;

    while(units<6 && nLight>0 && sumUnits>0){
        if (unit_count[units]==0){
            units++;
            continue;
        } else {
            if(min<units){
                min=units;
            }
            unit_count[units]--;
            running[units]++;
            nLight--;
            running_light+=units;
            sumUnits-=running_light;
        }
    }

    bestgradeGWA = range->best*running_light;
    worstgradeGWA = range->worst*running_heavy;

    float remainder = target - tempGWA - bestgradeGWA - worstgradeGWA;

    float minGWA = ((int) (4 * remainder/sumUnits))/4.0;

    if(minGWA<1){
        printf("It is impossible to attain your target GWA with the parameters set.\nKindly try again");
        return;
    }

    float rem_coursegrade[6]={0};

    for(int i=1; i<6; i++){
        if(unit_count[i]!=0){
            rem_coursegrade[i]= minGWA;
        }
    }

    printf("Given the targetted GWA %.3f for the ", gwa);
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
    printf("semester of the %s A.Y. and the inputted paramters,\none must achieve the following grades:", ay);

    int hasprint=0;
    for(int i=0; i<size;i++){
        if(arr[i].grade!=0){
            printf("\n A %0.2f in %s",arr[i].grade, arr[i].code);
            hasprint =1;
        }
        if(i==size && hasprint==1){
            printf("\nAs well as achieve the following grades:");
        }
    }
    for(int i=0;i<6;i++){
        if(unit_count[i]!=0 && running[i]==0){
            printf("\n For courses with %d units, one must have at the minimum, an average grade of %.2f.", i, rem_coursegrade[i]);
        }
        else if(running[i]!=0 && unit_count[i]==0){
            if(i==min && min!=max){
                printf("\n For courses with %d units, one must have at the minimum, an average grade of %.2f.", i, range->best);
            }
            else if(i==max && min!=max) {
                printf("\n For courses with %d units, one must have at the at the minimum, an average grade of %.2f.", i, range->worst);
            }
            else {
                printf("\n For courses with %d units, one must have a grade of %.2f at least %d times and a grade\n of %.2f at least %d times.", i,range->worst, running[i],range->best, running[i]-range->numWorst);
            }
        }
        else if(running[i]==0 && unit_count[i]==0){
            continue;
        }
        else{
            if(i!=max && max!=min){
                printf("\n For courses with %d units, one must have a grade of %.2f at least %d times and %.2f for every other\ncourse with the same amount of units.", i,range->best, running[i], rem_coursegrade[i]);
            }
            else if(i==max && max!=min) {
                printf("\n For courses with %d units, one must have a grade of %.2f at least %d times and %.2f for every other\ncourse with the same amount of units.", i,range->worst, running[i], rem_coursegrade[i]);
            } else {
                printf("\n For courses with %d units, one must have a grade of %.2f at least %d times, a grade\n of %.2f at least %d times and an average grade of %.2f for every other course with the same amount of units.", i,range->worst, running[i],range->best, running[i]-range->numWorst, rem_coursegrade[i]);
            }
        }
    }
}

// menu for gwa forecasting. allows users to set parameters in order to forecast their gwa
void gwaForecast(FILE* fp){
    courseProfile c;
    minmaxGrade range={0};
    int option, ans, read, id=0, count=0;
    char buffer[100];
    int sems;
    char ay[10];
    float fGWA, ftemp;

    for(;;){
        printf("Enter the academic school year and semester you wish to forecast the GWA for:\n");
        while (fgets(buffer,sizeof(buffer),stdin) && buffer[0] == '\n');
        read = sscanf(buffer,"%s %d", ay, &sems);

        if(read!=2){
            printf("\nError. Inputs must be in the format 20XX-20XX 2\nTry again.\n\n");
            continue;
        }
        if(courseCounter(fp,ay,sems)<1){
            printf("\nError. There are no courses in the inputted semester. Try again.\n\n");
            continue;
        }
        break;
    }

    for(;;){
        printf("Enter the target GWA for that semester:\n");
        read = scanf("%f",&fGWA);

        if(flush_stdin() || read < 1 || fGWA < 1){
            printf("Error! Invalid input. Inputs must be a number between 1.00 and 3.00. Try again.\n\n");
            continue;
        }

        if(fGWA>3.00 && fGWA<5.00){
            printf("A GWA lesser than 3.00 means failing several subjects. Aim higher.\n\n");
            continue;
        }

        if(fGWA==1.00){
            printf("In order to get a GWA of 1.00, you will have to get a grade of 1.00 in every course you are taking\nin that semester.\n\n");
            return;
        }
        break;
    }

    int num = courseCounter(fp, ay, sems);
    courseProfile arr[num];

    fseek(fp,0, SEEK_SET);

    while(!feof(fp)) {
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


        if(strcmp(ay,c.aYear)==0 && sems==c.sem){
            strcpy(arr[count].aYear, ay);
            arr[count].sem = sems;
            strcpy(arr[count].code, c.code);
            arr[count].units= c.units;
            arr[count].grade= c.grade;
            count++;
        }
    }
        printf("\n*****************************\n\n");
    do {
        printf("GWA FORECASTER MENU");
        printf("\nPlease Select an Option\n");
        printf("(1) Set temporary grades\n");
        printf("(2) Set the maximum and minimum grades\n");
        printf("(3) Forecast GWA\n");
        printf("(0) Return to main menu\n");
        printf("\n*****************************\n");

        for(;;){
            printf("ENTER CHOICE: ");
            while (fgets(buffer,sizeof(buffer),stdin) && buffer[0] == '\n');
            int read = sscanf(buffer,"%d",&option);

            if(!read){
                printf("\nError. Please input a number.\n\n");
                continue;
            }
            break;
        }

        printf("*****************************");

        switch (option) {
            case 1:
                do{
                    printf("\n\n\nID A.Y. SEMESTER COURSE UNITS GRADE\n");
                    for(int i=0; i<num; i++){
                        printf("%d %s %d %s %d %0.2f\n", i+1, arr[i].aYear, arr[i].sem, arr[i].code, arr[i].units, arr[i].grade);
                    }
                    for(;;){
                        printf("\nEnter the ID of the course you wish to temporarily set a grade for: ");
                        read = scanf("%d", &id);

                        if(flush_stdin() || read < 1 || id>num || id<1){
                            printf("Error! Invalid input. Inputs must be a number between 1 and %d. Try again.\n\n", num);
                            continue;
                        }
                        break;
                    }
                    for(;;){
                        printf("\nEnter the grade you wish to temporarily set: ");
                        read = scanf("%f", &ftemp);

                        if(flush_stdin() || read < 1){
                            printf("Error! Invalid input. Inputs must be a number between 1.00 and 5.00. Try again.\n\n");
                            continue;
                        }
                        if(ftemp>3.00 && ftemp<5.00){
                            printf("Error! Invalid input. A grade in the range (3.00,5.00) does not exist in the UP grading system.\nA grade of 4.0 does exist, however, it is a temporary classification.\n\n");
                            continue;
                        }

                        if((ftemp<1.00 || ftemp>5.00) || !(isValidGrade(ftemp))){
                            printf("Error! Invalid input. The UP grading system uses numbers in 0.25 increments. Try again\n\n");
                            continue;
                        }
                        break;
                    }

                    arr[(id-1)].grade= ftemp;
                    for(;;){
                        printf("\nDo you wish to continue inputting grades? Input 1 if yes, 0 if no. \n");
                        read = scanf("%d", &ans);
                        if(flush_stdin() || read < 1 || (ans!=1 && ans!= 0)){
                            printf("Error! Invalid input. Inputs must either be 1 for yes or 0 for no. Try again.\n\n");
                            continue;
                        }
                        break;
                    }
                }while(ans!=0);
                break;

            case 2:
                for(;;){
                    printf("\n\n\nEnter your best possible grade: ");
                    read = scanf("%f", &range.best);

                    if(flush_stdin() || read < 1){
                        printf("\nError! Invalid input. Inputs must be a number between 1.00 and 5.00. Try again.\n\n");
                        continue;
                    }

                    if(range.best>3.00 && range.best<5.00){
                        printf("Error! Invalid input. A grade in the range (3.00,5.00) does not exist in the UP grading system.\nA grade of 4.0 does exist, however, it is a temporary classification.\n\n");
                        continue;
                    }

                    if((range.best<1.00 || range.best>5.00) || (((int)(100*range.best))%25)!=0){
                        printf("\nError! Invalid input. The UP grading system uses numbers in 0.25 increments. Try again\n\n");
                        continue;
                    }

                    break;
                }
                for(;;){
                    printf("How many classes at least will have that grade?\n");
                    read=scanf("%d", &range.numBest);

                    if(flush_stdin() || read < 1 || range.numBest>num){
                        printf("\nError! Invalid input. Inputs must be a number between 1 and %d. Try again.\n\n", num);
                        continue;
                    }
                    break;
                }

                for(;;){
                    printf("Enter your worst possible grade: ");
                    read = scanf("%f", &range.worst);

                    if(flush_stdin() || read < 1){
                        printf("\nError! Invalid input. Inputs must be a number between 1.00 and 5.00. Try again.\n\n");
                        continue;
                    }

                    if(range.worst>3.00 && range.worst<5.00){
                        printf("Error! Invalid input. A grade in the range (3.00,5.00) does not exist in the UP grading system.\nA grade of 4.0 does exist, however, it is a temporary classification.\n\n");
                        continue;
                    }

                    if((range.worst<1.00 || range.worst>5.00) || (((int)(100*range.worst))%25)!=0){
                        printf("\nError! Invalid input. The UP grading system uses numbers in 0.25 increments. Try again\n\n");
                        continue;
                    }
                    break;
                }
                for(;;){
                    printf("How many classes at least will have that grade?\n");
                    read = scanf("%d", &range.numWorst);
                    if(flush_stdin() || read < 1 || range.numWorst>(num-range.numBest)){
                        printf("\nError! Invalid input. Inputs must be a number between 1 and %d. Try again.\n",num-range.numBest);
                        continue;
                    }

                    break;
                }
                break;
            case 3:
                printf("\n\n\n");
                forecaster(ay,sems,fGWA,arr,num,&range);
                printf("\n\n");
                break;
            case 0:
                printf("\n\n\nReturning to main menu\n");
                break;
            default:
                printf("\n\n\nInvalid choice!\n");
                break;
        }

        printf("\n*****************************\n\n\n");

    } while (option != 0);

    return;

}


int main(void){
    FILE* fp = fopen("user.csv","r+");
    int choice;
    char buffer[100];
    if(!fp){
        fp = fopen("user.csv","w+");
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

        for(;;){
            printf("ENTER CHOICE: ");
            while (fgets(buffer,sizeof(buffer),stdin) && buffer[0] == '\n');
            int read = sscanf(buffer,"%d",&choice);

            if(!read){
                printf("\nError. Please input a number.\n");
                continue;
            }
            break;
        }

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
                gwaForecast(fp);
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
