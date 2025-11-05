#include <stdio.h>
#include <string.h>

#define MAX_ACTIVITIES 100
#define MAX_TAGS 500

typedef struct {
    char name[20];
    char tags[5][10];
    int tag_count;
} activity;

typedef struct {
    char name[10];
    int instances;
} tag;

void add_activity(char name[20], char tags[55]);
void summarize_activities();
void list_activities();
void generate_taglist();
int compare_with_base(char tag[10]);

static activity activities[MAX_ACTIVITIES];
static int activity_count = 0;
static tag tags_base[MAX_TAGS];
static int all_tag_count = 0;

int main() {
    printf("\033[1;34mWelcome to the habits version 0.5!\033[0m\n");
    while(true) {

        char cmd[20];
        char name[20];
        char tags[55];

        printf("\033[1;33mcmd> \033[0m");

        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strncmp(cmd, "new-activity", (size_t)12) == 0) {
            printf("\033[0;36mAdding new activity...\033[0m\n");
            printf("\033[0;36mEnter activity properties:\033[0m\n");
            printf("\033[1;33mname: \033[1;37m");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';
            printf("\033[1;33mtags: \033[1;37m");
            fgets(tags, sizeof(tags), stdin);
            tags[strcspn(tags, "\n")] = '\0';
            printf("\033[0m[DEBUG] Trying to add activity \"%s\" with tags: %s...\n", name, tags);
            add_activity(name, tags);
        } else if (strncmp(cmd, "list-activities", (size_t)15) == 0) {
            list_activities();
        } else if (strncmp(cmd, "exit", (size_t)4) == 0) {
            printf("\033[1;34mExiting habits, good day!\033[0m\n");
            return 0;
        } else if (strncmp(cmd, "help", (size_t)4) == 0) {
            printf("\033[0;36mCommands list for tododaily version 0.05:\033[0m\n");
            printf("\033[1;33mnew-activity: \033[0mAdd a new activity to the register.\n");
            printf("\033[1;33mlist-activities: \033[0mList all activities in the register which can be added to today's list.\n");
            printf("Note: For now all activities are automatically added to today's list.\n");
            printf("\033[1;33msummarize: \033[0mList all activities done today and count the tags.\n");
            printf("\033[1;33mhelp: \033[0mDisplay all possible commands.\n");
        } else if (strncmp(cmd, "summarize", (size_t)9) == 0) {
            summarize_activities();
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
}

void add_activity(char name[20], char tags[55]) {
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
            printf("[DEBUG] Added a new tag named %s.\n", tags_base[all_tag_count].name);
            all_tag_count +=1;
        } else if (is_unique == 0) {
            printf("[DEBUG] %s is not unique.\n", token);
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
    printf("\033[0;36mSummarizing today's activities...\033[0m\n");
    list_activities();
    int x;
    for (x = 0; x < all_tag_count; x++) {
        if (tags_base[x].instances < 2) {
            printf("\033[1;34m[%s]x%d] ", tags_base[x].name, tags_base[x].instances);
        } else if (tags_base[x].instances == 2) {
            printf("\033[1;33m[%s]x%d] ", tags_base[x].name, tags_base[x].instances);
        } else {
            printf("\033[1;35m[%s]x%d] ", tags_base[x].name, tags_base[x].instances);
        }
    }
    printf("\033[0m\n");

}

void generate_taglist() {
    printf("Generating the taglist ///deprecated///");
    int i;
    for (i = 0; i < activity_count; i++) {
        int x;
        for (x = 0; x < activities[i].tag_count; x++) {
            printf("%d\n",compare_with_base(activities[i].tags[x]));
        }
    }
}

int compare_with_base(char tag[10]) {
    printf("[DEBUG] Comparing %s...\n", tag);

    int i;
    for (i = 0; i < all_tag_count; i++) {
        if (strncmp(tag, tags_base[i].name, (size_t)10) == 0) {
            printf("[DEBUG] Compared %s with %s and think they are the same.\n", tag, tags_base[i].name);
            tags_base[i].instances += 1;
            return 0;
        }
        printf("[DEBUG] Compared %s with %s and think they are NOT the same.\n", tag, tags_base[i].name);
    }

    if (all_tag_count == 0) {
        return 1;
    }

    return 1;
}
