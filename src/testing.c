#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/claves.h"

#define MAX_VALUE1 256

typedef int (*FUNC_TEST)();

void format_time(char *time_str, int total_seconds) {
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    sprintf(time_str, "%02im ,%02is", minutes, seconds);
}

void execute_test(FUNC_TEST f_test, char *msg) {
    clock_t start, elapsed;
    start = clock();
    int res = f_test();
    elapsed = ((double)(clock() - start)) / CLOCKS_PER_SEC;

    char time_str[128];
    format_time(time_str, elapsed);

    if (res == 0) {
        printf("✅ Success %s => %s\n", msg, time_str);
    } else {
        printf("❌ Error %s => %s\n", msg, time_str);
    }
}

int test1_connection() {
    if (init() != 0) {
        printf("Error in init\n");
        return -1;
    } else {
        return 0;
    }
}

int test2_setValueSignalNonExistent(){
    int key = 0;
    char* value1 = "test2";
    int value2 = 2;
    double value3 = 10.1;

    if(set_value(key, value1, value2, value3)!=0){
        printf("Error in set_value\n");
        return -1;
    } else{
        return 0;
    }
}

int test3_setValueSignalExistent(){
    int key=0;
    char* value1="test3";
    int value2 = 3;
    double value3 = 20.2;

    if(set_value(key,value1,value2,value3)==0){
        printf("Error, setting a value when there is already one");
        return -1;
    }else{
        return 0;
    }
}

int test4_getExistentValue(){
    int key = 0;
    char* value1Get = malloc(MAX_VALUE1 * sizeof(char));
    char* existentValue1 = "test2";
    int value2Get;
    int existentValue2 = 2;
    double value3Get;
    double existentValue3 = 10.1;

    if(get_value(key, value1Get, &value2Get, &value3Get)!=0){
        printf("Error in get_value when the value is existant\n");
        return -1;
    }else{
        if (value2Get != existentValue2){
            printf("Integer values are not the same: %d!=%d, \n", value2Get,existentValue2);
            return -1;
        }else if (value3Get != existentValue3){
            printf("Double values are not the same: %lf!=%lf, \n", value3Get,existentValue3);
            return -1;
        }else if (strcmp(value1Get,existentValue1)){
            printf("String values are not the same: %s!=%s, \n", value1Get,existentValue1);
            return -1;
        }else{
            return 0;
        }
        
    }
}

int test5_getNonExistentValue(){
    int key = 1;
    char* value1Get = malloc(MAX_VALUE1 * sizeof(char));
    int value2Get;
    double value3Get;

    if(get_value(key, value1Get, &value2Get, &value3Get)==0){
        printf("Error in get_value when the value is non existant\n");
        return -1;
    }else{
        return 0;
    }

}

int test6_checkExistanceTrue(){
    int key = 0;
    int res = exist(key);
    if(res==1){
        return 0;
    }else if (res==0){
        printf("Error in exist, saying not existant when it exists\n");
        return -1;
    }else{
        printf("Error in exist\n");
        return -1;
    }
}

int test7_checkExistanceFalse(){
    int key = 1;
    int res = exist(key);
    if(res==0){
        return 0;
    }else if (res==1){
        printf("Error in exist, saying existant when it doesn't exist\n");
        return -1;
    }else{
        printf("Error in exist\n");
        return -1;
    }
}

int test8_modifyExistantValue(){
    int key=0;
    char* value1Modified = "test8";
    int value2Modified = 40;
    double value3Modified = 30.4;

    if (modify_value(key, value1Modified, value2Modified, value3Modified)==0){
        char* value1Retrieved = malloc(MAX_VALUE1 * sizeof(char));
        int value2Retrieved;
        double value3Retrieved;
        if(get_value(key, value1Retrieved, &value2Retrieved, &value3Retrieved)==-1){
            printf("Error in get_value after modifying\n");
            return -1;
        }else{
            if (value2Retrieved != value2Modified){
                printf("Integer values are not the same: %d!=%d, \n", value2Retrieved,value2Modified);
                return -1;
            }else if (value3Retrieved != value3Modified){
                printf("Double values are not the same: %lf!=%lf, \n", value3Retrieved,value3Modified);
                return -1;
            }else if (strcmp(value1Retrieved,value1Modified)){
                printf("String values are not the same: %s!=%s, \n", value1Retrieved,value1Modified);
                return -1;
            }else{
                return 0;
            }
        }

    }else{
        printf("Error when modifying the value\n");
        return -1;
    }
}

int test9_modifyNonExistantValue(){
    int key=1;
    char* value1Modified = "test9";
    int value2Modified = 40;
    double value3Modified = 30.4;

    if (modify_value(key, value1Modified, value2Modified, value3Modified)==-1){
        return 0;
    }else{
        printf("Error in Modify Value when the key is non existant\n");
        return -1;
    }
}

int test10_copyKeyNonExistentBoth(){
    int key = 1;
    int key2 = 2;

    if (copy_key(key,key2)!=-1){
        printf("Error when copying unexistent key to an unexistent key\n");
        return -1;
    }else{
        return 0;
    }
}

int test11_copyKeyFirstNonExistentSecondExistent(){
    int key = 1;
    int key2 = 0;
    if (copy_key(key,key2)!=-1){
        printf("Error when copying unexistent key to an existent key\n");
        return -1;
    }else{
        return 0;
    }
}

int test12_copyKeyFirstExistentSecondNonExistent(){
    int key = 0;
    int key2 = 1;
    if (copy_key(key,key2)==-1){
        printf("Error when copying existent key to an unexistent key\n");
        return -1;
    }else{
        char value1key[MAX_VALUE1];
        char value1key2[MAX_VALUE1];
        int value2key;
        int value2key2;
        double value3key;
        double value3key2;

        int res1 = get_value(key, value1key, &value2key, &value3key);
        int res2 = get_value(key2, value1key2, &value2key2, &value3key2);

        if (res1==-1 || res2 ==-1){
            printf("Error when retrieving the values after copying them\n");
            return -1;
            
        }else{
            if (strcmp(value1key,value1key2)){
                printf("The copied values of the string are different: %s!=%s\n", value1key, value1key2);
                return -1;
            }else if (value2key!=value2key2){
                printf("The copied values of the integers are different: %i!=%i\n", value2key,value2key2);
                return -1;
            }else if (value3key!=value3key2){
                printf("The copied values of the doubles are different: %lf!=%lf\n", value3key, value3key2);
                return -1;
            }else{
                return 0;
            }
        }
    }
}

int test13_copyKeyBothExistentOriginalSameValues(){ //test to be changed to accept modifications
    int key = 0;
    int key2 = 1;
    if (copy_key(key,key2)==-1){
        printf("Error when copying existent key to an exitent key\n");
        return -1;
    }else{
        char value1key[MAX_VALUE1];
        char value1key2[MAX_VALUE1];
        int value2key;
        int value2key2;
        double value3key;
        double value3key2;

        int res1 = get_value(key, value1key, &value2key, &value3key);
        int res2 = get_value(key2, value1key2, &value2key2, &value3key2);

        if (res1==-1 || res2 ==-1){
            printf("Error when retrieveing the values after copying them\n");
            return -1;
            
        }else{
            if (strcmp(value1key,value1key2)){
                printf("The copied values of the string are different: %s!=%s\n", value1key, value1key2);
                return -1;
            }else if (value2key!=value2key2){
                printf("The copied values of the integers are different: %i!=%i\n", value2key,value2key2);
                return -1;
            }else if (value3key!=value3key2){
                printf("The copied values of the doubles are different: %lf!=%lf\n", value3key, value3key2);
                return -1;
            }else{
                return 0;
            }
        }
    }
}

int test14_copyKeyBothExistentOriginalDifValues(){
    int key = 2;
    int key2 = 0;
    char* value1 = "test14";
    int value2 = 14;
    double value3 = 14.14;

    if(set_value(key, value1, value2, value3)!=0){
        printf("Error in set_value\n");
        return -1;
    }

    if (copy_key(key,key2)==-1){
        printf("Error when copying existent key to an exitent key\n");
        return -1;
    }else{
        char value1key2[MAX_VALUE1];
        int value2key2;
        double value3key2;
        char value1key[MAX_VALUE1];
        int value2key;
        double value3key;

        int res1 = get_value(key, value1key, &value2key, &value3key);
        int res2 = get_value(key2, value1key2, &value2key2, &value3key2);

        if (res2 ==-1){
            printf("Error when retrieveing the values after copying them\n");
            return -1;
            
        }else if(res1==-1){
            printf("Error when retrieveing the created key \n");
            return -1;
        }else{
            if (strcmp(value1,value1key2)){
                printf("The copied values of the string are different: %s!=%s\n", value1, value1key2);
                return -1;
            }else if (value2!=value2key2){
                printf("The copied values of the integers are different: %i!=%i\n", value2,value2key2);
                return -1;
            }else if (value3!=value3key2){
                printf("The copied values of the doubles are different: %lf!=%lf\n", value3, value3key2);
                return -1;
            }else{
                return 0;
            }
        }
    }
}

int test15_deleteExistentKey(){
    int key=2;
    if (delete_key(key)==-1){
        printf("Error when deleting the key\n");
        return -1;
    }else{
        if(exist(key)==1){
            printf("Key still exists\n");
            return -1;
        }else if(exist(key)==-1){
            printf("Error when seeing if key exists\n");
            return -1;
        }
        return 0;
    }
}

int test16_deleteNonExistentKey(){
    int key=2;
    if (delete_key(key)!=-1){
        printf("Error when deleting a non existent key\n");
        return -1;
    }else{
        return 0;
    }
}

int test17_cleanupAfterInit(){
    int key=0;
    int key2=1;
    char value1key[MAX_VALUE1];
    int value2key;
    double value3key;

    if (init()==-1){
        return -1;
    }else{
        if (get_value(key, value1key, &value2key, &value3key)!=-1){
            printf("Error, getting a value that should have been deleted\n");
            return -1;
        }
        if (exist(key)!=0){
            printf("Error saying that the key doesn't exist\n");
            return -1;
        }
        if (delete_key(key)!=-1){
            printf("Error, trying to delete a non existent key\n");
            return -1;
        }
        if (copy_key(key,key2)!=-1){
            printf("Error, trying to copy a non existent key\n");
            return -1;
        }
        if (modify_value(key,"test17", 17, 17.17)!=-1){
            printf("Error, trying to modify a non existent value");
            return -1;
        }
        return 0;
    }
}


int main(int argc, char* argv[]) {
    execute_test(test1_connection, "1. Connection");
    execute_test(test2_setValueSignalNonExistent, "2. Set value (correctness checked in 4.)");
    execute_test(test3_setValueSignalExistent, "3. Set value when key already exists");
    execute_test(test4_getExistentValue, "4. Get Value that already exists");
    execute_test(test5_getNonExistentValue, "5. Get Value that doesn't exist");
    execute_test(test6_checkExistanceTrue, "6. Exist when the key exist");
    execute_test(test7_checkExistanceFalse, "7. Exist when the key doesn't exist");
    execute_test(test8_modifyExistantValue, "8. Modify the values when existant");
    execute_test(test9_modifyNonExistantValue, "9. Modify the values when non existant");
    execute_test(test10_copyKeyNonExistentBoth, "10. Copy key when none of the values exist");
    execute_test(test11_copyKeyFirstNonExistentSecondExistent, "11. Copy key when key to copy doesn't exist and destination exists");
    execute_test(test12_copyKeyFirstExistentSecondNonExistent, "12. Copy key when the key to copy exist and not the destination");
    execute_test(test13_copyKeyBothExistentOriginalSameValues, "13. Copy key when the key to copy exist and the destination exists, and originally they have the same values");
    execute_test(test14_copyKeyBothExistentOriginalDifValues, "14. Copy key when the key to copy exist and the destination exists, and originally they have different values");
    execute_test(test15_deleteExistentKey,"15. Delete a key that exists");
    execute_test(test16_deleteNonExistentKey,"16. Delete a key when it doesn't exist");
    execute_test(test17_cleanupAfterInit, "17. Trying to perform access operations after deleting the whole list");
}