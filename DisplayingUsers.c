// C program for displaying the users currently logged into the system

#include <pwd.h>
#include <utmpx.h>
#include <stdio.h>

int main()
{
	struct utmpx *user = getutxent();
	struct passwd *uid;
	if(user == EMPTY)
	{
        printf("No record found: Returned value: %p\n", user);
        return -1;
	}
	
	printf("User, UID\n");
	for(user; user != EMPTY; user = getutxent())
	{
        if(user->ut_type == USER_PROCESS)
        {
        	uid = getpwnam(user->ut_user);
            printf("%s, ", user->ut_user);
            if(uid != NULL)
            {
                printf("%d\n", uid->pw_uid);
            }
        }
	}
	return 0;
}
