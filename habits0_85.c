#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_ACTIVITIES 100
#define MAX_TAGS 500

static int debug = 0;

typedef struct {
    char name[21];
    char tags[5][11];
    int tag_count;
    int times[7];
} activity;

typedef struct {
    char name[11];
    int instances;
} tag;

typedef struct {
    int id;
    int duration;
} done;

typedef struct {
    char name[11];
    int num;
    int duration;
    int instances;
} today_tag;

void add_activity(char name[21], char tags[55]);
void summarize_activities();
void list_activities();
void generate_taglist();
int compare_with_base(char tag[10]);
void save_to_file();
void read_from_file();
void delete_activity(int id);
void edit_activity(int id);
void today_done(int id, int duration);
void manage_times(char line[100]);
void summarize_week();

static activity activities[MAX_ACTIVITIES];
static int activity_count = 0;
static tag tags_base[MAX_TAGS];
static int all_tag_count = 0;
static done today_done_act[MAX_ACTIVITIES];
static int all_today_done;
static today_tag todays_tags[MAX_TAGS];
static int all_todays_tags;
static time_t readTime;
static int timeDifference;
static int day_diff;

int main() {
    time_t currentTime = time(NULL);

    printf("\033[1;34mWelcome to the habits version 0.85!\033[0m\n");
    if (debug == 1) {printf("[DEBUG] Current timestamp is: %d\n", currentTime);}
    if (debug == 1) {printf("[DEBUG] Attempting to find save file...\n");}
    FILE *rptr;
    rptr = fopen("habits.hbts", "r");
    if(rptr == NULL) {
        printf("\033[31mDidn't find a save file. Please check your current directory.\033[0m\n");
        printf("\033[31mIgnore this if you want to start a new file.\033[0m\n");
    } else {
        printf("\033[32mFound the save file, loading...\033[0m\n");
        read_from_file();
    }

    /*timeDifference = currentTime - readTime;
    if (debug == 0) {printf("[DEBUG] The time difference is: %d\n", timeDifference);}

    time_t today = time(NULL);
    struct tm td = *localtime(&today);
    td.tm_hour = 0;
    td.tm_min = 0;
    td.tm_sec = 0;
    td.tm_isdst = -1;
    time_t today_midnight = mktime(&td);
    if (debug == 0) {printf("[DEBUG] Today's midnight's timestamp: %ld\n", today_midnight);}

    struct tm rt = *localtime(&readTime);
    rt.tm_hour = 0;
    rt.tm_min = 0;
    rt.tm_sec = 0;
    rt.tm_isdst = -1;
    time_t read_midnight = mktime(&rt);
    if (debug == 0) {printf("[DEBUG] Read Timestamp's midnight's timestamp: %ld\n", read_midnight);}

    double seconds = difftime(today_midnight, read_midnight);
    int days = (int)(seconds / 86400);
    day_diff = days;

    if (debug == 0) {printf("[DEBUG] The day difference is: %d\n", days);}*/

    while(true) {

        char cmd[20];
        char name[21];
        char tags[55];
        char buffer[10];
        int id = 1000;

        printf("\033[1;33mcmd> \033[0m");

        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strncmp(cmd, "new-activity", (size_t)12) == 0) {
            printf("\033[0;36mAdding new activity...\033[0m\n");
            printf("\033[0;36mEnter activity properties:\033[0m\n");
            printf("\033[1;33mname: \033[1;37m");
//fgets(name, sizeof(name), stdin);
//name[strcspn(name, "\n")] = '\0';
            if (fgets(name, sizeof(name), stdin)) {
                if (strchr(name, '\n') == NULL) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                }
                name[strcspn(name, "\n")] = '\0';
            }
            printf("\033[1;33mtags: \033[1;37m");
//fgets(tags, sizeof(tags), stdin);
//tags[strcspn(tags, "\n")] = '\0';
            if (fgets(tags, sizeof(tags), stdin)) {
                if (strchr(tags, '\n') == NULL) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                }
                tags[strcspn(tags, "\n")] = '\0';
            }
            if (debug == 1) {printf("\033[0m[DEBUG] Trying to add activity \"%s\" with tags: %s...\n", name, tags);}
            add_activity(name, tags);

        } else if (strncmp(cmd, "list-activities", (size_t)15) == 0) {
            list_activities();

        } else if (strncmp(cmd, "exit", (size_t)4) == 0) {

            char answer;
            int c;
            printf("Don't forget to save. Please confirm exit (y/n): ");
            answer = getchar();
            while ((c = getchar()) != '\n' && c != EOF);
            if (answer == 'y' || answer == 'Y') {
                printf("\033[1;34mExiting habits, have a good day!\033[0m\n");
                return 0;
            }

        } else if (strncmp(cmd, "remove-activity", (size_t)15) == 0) {
            list_activities();
            printf("\033[0;36mEnter activity ID to remove: \033[0m");
            if (fgets(buffer, sizeof(buffer), stdin)) {
                buffer[strcspn(buffer, "\n")] = '\0';
                if (sscanf(buffer, "%d", &id) == 1){
                    delete_activity(id);
                } else {
                    printf("Invalid integer.\n");
                }
            }

        } else if (strncmp(cmd, "today-done", (size_t)15) == 0) {
            list_activities();
            int duration = 0;
            printf("\033[0;36mEnter activity ID that you did: \033[0m");
            if (fgets(buffer, sizeof(buffer), stdin)) {
                buffer[strcspn(buffer, "\n")] = '\0';
                if (sscanf(buffer, "%d", &id) == 1){
                    if (id < 1 || id > activity_count) {
                        printf("Invalid activity ID.\n");
                    } else {
                        if (debug == 1) {printf("[DEBUG] id is now %d.\n", id);}
                        printf("\033[0;36mEnter duration of the activity: \033[0m");
                        if (fgets(buffer, sizeof(buffer), stdin)) {
                            buffer[strcspn(buffer, "\n")] = '\0';
                            if (sscanf(buffer, "%d", &duration) == 1){
                                today_done(id, duration);
                                printf("\033[0;32mActivity %s is now done for %d minutes.\033[0\n", activities[id-1].name, today_done_act[id-1].duration);
                            } else {
                                printf("Invalid integer.\n");
                            }
                        }
                    }
                } else {
                    printf("Invalid integer.\n");
                }
            }

        } else if (strncmp(cmd, "edit-activity", (size_t)13) == 0) {
            list_activities();
            printf("\033[0;36mEnter activity ID to edit: \033[0m");
            if (fgets(buffer, sizeof(buffer), stdin)) {
                buffer[strcspn(buffer, "\n")] = '\0';
                if (sscanf(buffer, "%d", &id) == 1){
                    edit_activity(id);
                } else {
                    printf("Invalid integer.\n");
                }
            }

        }  else if (strncmp(cmd, "help", (size_t)4) == 0) {
            printf("\033[0;36mCommands list for habits version 0.85:\033[0m\n");
            printf("\033[1;33mnew-activity: \033[0mAdd a new activity to the register.\n");
            printf("\033[1;33mremove-activity: \033[0mRemove an activity from the register.\n");
            printf("\033[1;33medit-activity: \033[0mEdit an activity in the register.\n");
            printf("\033[1;33mlist-activities: \033[0mList all activities in the register which can be added to today's list.\n");
            printf("\033[1;33mtoday-done: \033[0mAdd an activity to today's list or increase its time.\n");
            printf("\033[1;33msummarize: \033[0mList all activities done today and count the tags.\n");
            printf("\033[1;33msum-week: \033[0mSummarize all activities's times from last 7 days.\n");
            printf("\033[1;33msave: \033[0mSave current list to a file in the current directory.\n");
            printf("\033[31mNote: Only saves the library entries for now\033[0m\n");
            printf("\033[1;33mhelp: \033[0mDisplay all possible commands.\n");
            printf("\033[1;33mdebug: \033[0mToggle debug mode.\n");

        } else if (strncmp(cmd, "summarize", (size_t)9) == 0) {
            summarize_activities();

        } else if (strncmp(cmd, "sum-week", (size_t)8) == 0) {
            summarize_week();

        } else if (strncmp(cmd, "save", (size_t)4) == 0) {
            save_to_file();

        } else if (strncmp(cmd, "debug", (size_t)5) == 0) {
            if (debug == 0) {
                debug = 1;
                printf("Debugging mode in now active.\n");
            } else if (debug == 1) {
                debug = 0;
                printf("Debugging mode is now turned off.\n");
            }

        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
}

void add_activity(char name[21], char tags[55]) {
    strcpy(activities[activity_count].name, name);
    activities[activity_count].tag_count = 0;
    int count = 0;
    char *token;
    token = strtok(tags, " ");
    while (token != NULL) {
        strcpy(activities[activity_count].tags[activities[activity_count].tag_count], token);
        int is_unique = compare_with_base(activities[activity_count].tags[activities[activity_count].tag_count]);
//        printf("%d\n",is_unique);
        if (is_unique == 1) {
            strcpy(tags_base[all_tag_count].name, token);
            tags_base[all_tag_count].instances = 1;
            if (debug == 1) {printf("[DEBUG] Added a new tag named %s.\n", tags_base[all_tag_count].name);}
            all_tag_count +=1;
        } else if (is_unique == 0) {
            if (debug == 1) {printf("[DEBUG] %s is not unique.\n", token);}
        }
        token = strtok(NULL, " ");
        activities[activity_count].tag_count += 1;
    }
    printf("\033[0;32mThe activity \033[1;32m<%s>\033[0;32m was added.\033[0m\n", name);
    activity_count += 1;
}

void list_activities() {
    printf("\033[0;36mListing all activities...\n");
    printf("Found %d entries.\033[0m\n", activity_count);
    int count = 0;
    int i;
    for (i = 0; i < activity_count; i++) {
        char tags[55];
        tags[0] = '\0';
        int x;
        for (x = 0; x < activities[i].tag_count; x++) {
            strcat(tags, activities[i].tags[x]);
            strcat(tags, " ");
        }
        printf("\033[1;37m#%d:\033[1;32m %s \033[0mwith tags \033[0;33m%s \033[0m\n", i+1, activities[i].name, tags);
    }
}

void summarize_activities() {
    /*printf("\033[0;36mSummarizing today's activities...\033[0m\n");
    list_activities();
    int x;
    for (x = 0; x < all_tag_count; x++) {
        if (tags_base[x].instances < 2) {
            printf("\033[1;34m[%s]x%d] ", tags_base[x].name, tags_base[x].instances);
        } else if (tags_base[x].instances == 2) {
            printf("\033[1;35m[%s]x%d] ", tags_base[x].name, tags_base[x].instances);
        } else {
            printf("\033[1;33m[%s]x%d] ", tags_base[x].name, tags_base[x].instances);
        }
    }
    printf("\033[0m\n");*/

    // Reset todays_tags
    int h;
    for (h = 0; h < all_todays_tags; h++){
        strcpy(todays_tags[h].name,"\0");
        todays_tags[h].instances = 0;
        todays_tags[h].duration = 0;
    }
    all_todays_tags = 0;


    // List all the activities done today
    printf("\033[0;36mSummarizing today's activities...\033[0m\n");
    printf("\033[0;36mActivities:\033[0m\n");
    int x;
    int y;
    int z;
    int isUnique;
    for (x = 0; x < all_today_done; x++) {
        // Write the activity's name and its duration
        printf("\033[1;32m%s: \033[1;34m%d minutes\033[0m\n", activities[today_done_act[x].id-1].name, today_done_act[x].duration);
        // Add the activity's tags to todays_tags
        for (y = 0; y < activities[today_done_act[x].id-1].tag_count; y++) {
            isUnique = 1;
            for (z = 0; z < all_todays_tags; z++) {
                if (strncmp(activities[today_done_act[x].id-1].tags[y], todays_tags[z].name, (size_t)11) == 0) {
                    if (debug == 1) {printf("[DEBUG] Compared %s with %s and think they are the same.\n", activities[today_done_act[x].id-1].tags[y], todays_tags[z].name);}
                    todays_tags[z].instances += 1;
                    if (debug == 1) {printf("[DEBUG] Added one instance to %s\n", todays_tags[z].name);}
                    todays_tags[z].duration += today_done_act[x].duration;
                    isUnique = 0;
                } else {
                    if (debug == 1) {printf("[DEBUG] Compared %s with %s and think they are NOT the same.\n", activities[today_done_act[x].id-1].tags[y], todays_tags[z].name);}
                    //strcpy(dest, source);
                }
            }
            if (isUnique == 1) {
                if (debug == 1) {printf("[DEBUG] I think the tag %s is unique, adding it to todays_tags\n", activities[today_done_act[x].id-1].tags[y]);}
                strcpy(todays_tags[all_todays_tags].name, activities[today_done_act[x].id-1].tags[y]);
                todays_tags[all_todays_tags].instances ++;
                todays_tags[all_todays_tags].duration += today_done_act[x].duration;
                all_todays_tags ++;
            }
        }
    }

    // List all today's tags with their durations and instances
    int i;
    printf("\033[0;36mTags:\033[0m\n");
    for (i = 0; i < all_todays_tags; i++) {
        printf("\033[1;31m[%s] \033[0;36min %d instances \033[0;34mfor %d minutes\033[0m\n", todays_tags[i].name, todays_tags[i].instances, todays_tags[i].duration);
    }

}

/*void generate_taglist() {
    printf("Generating the taglist ///deprecated///");
    int i;
    for (i = 0; i < activity_count; i++) {
        int x;
        for (x = 0; x < activities[i].tag_count; x++) {
            printf("%d\n",compare_with_base(activities[i].tags[x]));
        }
    }
}*/

int compare_with_base(char tag[10]) {
    if (debug == 1) {printf("[DEBUG] Comparing %s...\n", tag);}

    int i;
    for (i = 0; i < all_tag_count; i++) {
        if (strncmp(tag, tags_base[i].name, (size_t)10) == 0) {
            if (debug == 1) {printf("[DEBUG] Compared %s with %s and think they are the same.\n", tag, tags_base[i].name);}
            tags_base[i].instances += 1;
            return 0;
        }
        if (debug == 1) {printf("[DEBUG] Compared %s with %s and think they are NOT the same.\n", tag, tags_base[i].name);}
    }

    if (all_tag_count == 0) {
        return 1;
    }

    return 1;
}

void save_to_file() {
    if (debug == 1) {printf("[DEBUG] Saving to file...\n");}
    FILE *fptr;
    fptr = fopen("habits.hbts", "w");

    if (fptr == NULL) {
        printf("Error opening file!\n");
    }

    time_t saveTime = time(NULL);

    fprintf(fptr, "# Habits save file made with version higher than 0.81\n");
    fprintf(fptr, "# Feel free to change it directly here\n");
    fprintf(fptr, "# Make sure not to disturb the structure though :D\n");
    fprintf(fptr, "\n");
    fprintf(fptr, "TIMESTAMP\n");
    fprintf(fptr, "%d\n", saveTime);
    fprintf(fptr, "\n");

    int i;
    for (i = 0; i < activity_count; i++) {
        fprintf(fptr, "%s\n%s %s %s %s %s\n%d %d %d %d %d %d %d\n", activities[i].name, activities[i].tags[0], activities[i].tags[1], activities[i].tags[2], activities[i].tags[3], activities[i].tags[4],today_done_act[i].duration,activities[i].times[1],activities[i].times[2],activities[i].times[3],activities[i].times[4],activities[i].times[5],activities[i].times[6]);

        if (debug == 1) {printf("[DEBUG] Printing %s %s %s %s %s %s to the file...\n", activities[i].name, activities[i].tags[0], activities[i].tags[1], activities[i].tags[2], activities[i].tags[3], activities[i].tags[4]);}
    }

    printf("\033[0;32mSuccesfully saved to a habits.hbts file on current directory.\033[0m\n");
    fclose(fptr);
}

void read_from_file(){
    if (debug == 1) {printf("[DEBUG] Attempting to read from file...\n");}
    FILE *fptr;
    fptr = fopen("habits.hbts", "r");
    char readLine[100];
    int i = 0;
    int x = 0;
    time_t timestamp = 0;

    while (fgets(readLine, 100, fptr)) {
        if (debug == 1) {printf("[DEBUG] Now reading line #%d\n", i);}
        char readName[21];
        char readTags[55];

        if (i==5) {
            if (debug == 1) {printf("[DEBUG] Line %d is probably the timestamp.\n", i);}
            readLine[strcspn(readLine, "\n\r")] = 0;
            sscanf(readLine, "%ld", &timestamp);
            readTime = timestamp;
            if (debug == 1) {printf("[DEBUG] The readTime is: %ld\n", (long)readTime);}
            time_t currentTime = time(NULL);
            if (debug == 1) {printf("[DEBUG] Current timestamp is: %d\n", currentTime);}
            timeDifference = currentTime - readTime;
            if (debug == 1) {printf("[DEBUG] The time difference is: %d\n", timeDifference);}

            time_t today = time(NULL);
            struct tm td = *localtime(&today);
            td.tm_hour = 0;
            td.tm_min = 0;
            td.tm_sec = 0;
            td.tm_isdst = -1;
            time_t today_midnight = mktime(&td);
            if (debug == 1) {printf("[DEBUG] Today's midnight's timestamp: %ld\n", today_midnight);}

            struct tm rt = *localtime(&readTime);
            rt.tm_hour = 0;
            rt.tm_min = 0;
            rt.tm_sec = 0;
            rt.tm_isdst = -1;
            time_t read_midnight = mktime(&rt);
            if (debug == 1) {printf("[DEBUG] Read Timestamp's midnight's timestamp: %ld\n", read_midnight);}

            double seconds = difftime(today_midnight, read_midnight);
            int days = (int)(seconds / 86400);
            day_diff = days;

            if (debug == 1) {printf("[DEBUG] The day difference is: %d\n", days);}
        }


        //(sscanf(buffer, "%d", &id)

        if (i > 6) {
            if (x == 0) {
                if (debug == 1) {printf("[DEBUG] Line %d is probably an activity name.\n", i);}
                readLine[strcspn(readLine, "\n\r")] = 0;
                strcpy(readName, readLine);
                if (debug == 1) {printf("[DEBUG] readName is now %s.\n", readName);}
                x = 1;
            } else if (x == 1){
                if (debug == 1) {printf("[DEBUG] Line %d is probably a string of tags.\n", i);}
                readLine[strcspn(readLine, "\n\r")] = 0;
                strcpy(readTags, readLine);
                x = 2;
            } else if (x == 2){
                if (debug == 1) {printf("[DEBUG] Line %d is probably a string of timestamps.\n", i);}
                readLine[strcspn(readLine, "\n\r")] = 0;
                if (timeDifference < 604800){
                    if (debug == 1) {printf("[DEBUG] timeDifference is smaller than 604800, managing times.\n", i);}
                    manage_times(readLine);
                }
                add_activity(readName, readTags);
                x = 0;
            }
        }

        i +=1;
    }
}

void delete_activity(int id){

    if (id < 1 || id > activity_count) {
        printf("Invalid activity ID.\n");
        return;
    }

    int x = id - 1;
    char answer;
    int c;
    printf("\033[31mDeleting the activity %s...\033[0m\n", activities[x].name);
    printf("\033[31mIf you save after deletion, all data about this activity will be deleted.\033[0m\n");
    printf("Please confirm (y/n): ");
    answer = getchar();
    while ((c = getchar()) != '\n' && c != EOF);

    if (answer == 'y' || answer == 'Y') {
        if (debug == 1) {printf("[DEBUG] Proceeding with deletion...\n");}

        // Find out if the activity was done today
        int k;
        for (k = 0; k < all_today_done; k++){
            if (today_done_act[k].id == id){
                int l;
                for (l = k; l < all_today_done - 1; l++){
                    today_done_act[l].id = today_done_act[l+1].id;
                    today_done_act[l].duration = today_done_act[l+1].duration;
                }
                all_today_done = all_today_done - 1;
            }
        }

        for (int i = x; i < activity_count - 1; i++) {
            if (debug == 1) {printf("[DEBUG] Copying %s to %s...\n",activities[i+1].name, activities[i].name);}
            activities[i] = activities[i + 1];
        }
        activity_count--;
        printf("\033[0;32mActivity removed.\033[0m\n");
        //printf("Please save and restart the program for summarize function to work, sorry for inconvenience...\n");
/*        printf("[DEBUG] Resetting the activity properties...\n");
        activities[x].name[0] = '\0';
        for (int i = 0; i < 5; i++) {
            activities[x].tags[i][0] = '\0';
        }
        printf("[DEBUG] Activity properties reset.\n");
        printf("[DEBUG] Shifting other activities...\n"); */
    }
    if (answer == 'n' || answer == 'N') {
        printf("No changes were made.\n");
    }
}

void edit_activity(int id){

    if (id < 1 || id > activity_count) {
        printf("Invalid activity ID.\n");
        return;
    }

    int x = id - 1;
    char newName[21];
    char newTags[55];

    printf("\033[33mEditing the activity %s...\033[0m\n", activities[x].name);

    printf("\033[0;36mEnter activity properties:\033[0m\n");
    printf("\033[1;33mnew name: \033[1;37m");

    if (fgets(newName, sizeof(newName), stdin)) {
        if (strchr(newName, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        newName[strcspn(newName, "\n")] = '\0';
    }
    printf("\033[1;33mnew tags: \033[1;37m");

    if (fgets(newTags, sizeof(newTags), stdin)) {
        if (strchr(newTags, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        newTags[strcspn(newTags, "\n")] = '\0';
    }

    printf("\033[0m");
    if (debug == 1) {printf("[DEBUG] Trying to edit activity %s\n", activities[x].name);}
    if (debug == 1) {printf("[DEBUG] New properties: %s with tags %s\n", newName, newTags);}
    if (debug == 1) {printf("[DEBUG] Recreating the entry...\n");}

    strcpy(activities[x].name, newName);
    if (debug == 1) {printf("[DEBUG] Activity ID %d name: %s\n", x + 1, activities[x].name);}

    activities[x].tag_count = 0;
    int count = 0;
    char *token;
    token = strtok(newTags, " ");
    while (token != NULL) {
        strcpy(activities[x].tags[activities[x].tag_count], token);
        int is_unique = compare_with_base(activities[x].tags[activities[x].tag_count]);
        //        printf("%d\n",is_unique);
        if (is_unique == 1) {
            strcpy(tags_base[all_tag_count].name, token);
            tags_base[all_tag_count].instances = 1;
            if (debug == 1) {printf("[DEBUG] Added a new tag named %s.\n", tags_base[all_tag_count].name);}
            all_tag_count +=1;
        } else if (is_unique == 0) {
            if (debug == 1) {printf("[DEBUG] %s is not unique.\n", token);}
        }
        token = strtok(NULL, " ");
        activities[x].tag_count += 1;
    }
    printf("\033[0;32mThe activity \033[1;32m<%s>\033[0;32m was added.\033[0m\n", activities[x].name);
    //printf("Please save and restart the program for summarize function to work, sorry for inconvenience...\n");
}

void today_done(int id, int duration){
    if (debug == 1) {printf("[DEBUG] Trying to add activity id %d with duration of %d...\n", id, duration);}
    int x;
    for (x = 0; x < all_today_done; x++) {
        if (today_done_act[x].id == id){
            today_done_act[x].duration = today_done_act[x].duration + duration;
            if (debug == 1) {printf("[DEBUG] As the activity was already done today, its duration was increased by %d minutes.\n", duration);}
            //printf("\033[0;32mActivity %s is now done for %d minutes.\033[0\n", activities[id-1].name, today_done_act[x].duration);
            return;
        }
    }

    today_done_act[all_today_done].id = id;
    today_done_act[all_today_done].duration = duration;
    all_today_done ++;
    if (debug == 1) {printf("[DEBUG] Added %d minutes to activity %s.\n", duration, activities[id-1].name);}
    //printf("\033[0;32mActivity %s is now done for %d minutes.\033[0\n", activities[id-1].name, today_done_act[x].duration);
}

void manage_times(char line[100]){
    if (debug == 1) {printf("[DEBUG] Line %s received.\n", line);}
    int time_day;
    int count = day_diff;
    if (debug == 1) {printf("count is %d", count);}
    char *token;
    token = strtok(line, " ");
    while (token != NULL) {
        if (sscanf(token, "%d", &time_day)){
            if (debug == 1) {printf("%d ", time_day);}
            if (count < 7) {activities[activity_count].times[count] = time_day;}
            if (count == 0) {today_done(activity_count + 1, time_day);}
            count +=1;
        }
        token = strtok(NULL, " ");
    }
}

void summarize_week(){
    for (int i = 0; i < activity_count; i++){
        printf("The activity %s was done for: \n", activities[i].name);
        printf("Today: %d\n", today_done_act[i].duration);
        printf("Yesterday: %d\n", activities[i].times[1]);
        printf("2 days ago: %d\n", activities[i].times[2]);
        printf("3 days ago: %d\n", activities[i].times[3]);
        printf("4 days ago: %d\n", activities[i].times[4]);
        printf("5 days ago: %d\n", activities[i].times[5]);
        printf("6 days ago: %d\n", activities[i].times[6]);
        printf("\n");
    }
}
