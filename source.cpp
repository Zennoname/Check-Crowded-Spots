#include <iostream>
using namespace std;

struct trajectory {
    int xCord;
    int yCord;
    int time_sec;
    trajectory* nextMove;
    int date;
    trajectory* marker;
};

struct multiUserTrajectory {
    trajectory* info;
    multiUserTrajectory* nextUser;
};

int DAYS_BEFORE = 5;
int USERS = 10;
int SIZE = 50;
int DAYS = 5;
int PATIENTS = 2;
bool ADMIN = true;
int SEARCH_TIME_START = 33442;
int SEARCH_TIME_END = 33500;
int REGION_OF_INTEREST[4] = { 10,10,15,15 };
int MIN_DURATION = 90;

//check if user has been infected
bool possible_covid_19_infection(trajectory* userTrajectory, int day, multiUserTrajectory* patientsTrajectory) {
    int r = 2; // distance from infected area that is considered dangerous
    int min_time = 4; // minimum minutes needed near infection to be infected
    int max_time = 10; // maximum minutes after a patient passes for the area to remain infected
    int time,distance;

    trajectory* tempList; //patient moves
    trajectory* userTemp; //user moves
    multiUserTrajectory* patientTemp = patientsTrajectory; //patient list
    while (patientTemp != NULL) {
        tempList = patientTemp->info; //for each patient
        while (tempList != NULL) {
            if (tempList->date == day) {
                time = 0; //for every move they make
                userTemp = userTrajectory;
                while (userTemp !=NULL && userTemp->date < day) {
                    userTemp = userTemp->nextMove;
                }
                while (userTemp != NULL) { //compare to every move the user makes
                    if ((userTemp->time_sec - tempList->time_sec) >= 0 && (userTemp->time_sec - tempList->time_sec) <= max_time * 60) {
                        distance = sqrt(pow((userTemp->xCord - tempList->xCord), 2) + pow((userTemp->yCord - tempList->yCord),2));
                        if (distance <= r) {
                            //if close enough to infected area count how long they stay in
                            time += 30;
                            cout << "Seconds spent near infected area: " << time << "\n";
                        }
                        else {
                            time = 0;
                            //cout << "Left area\n";
                        }
                    }
                    if (time >= min_time * 60) {
                        //if enough time spent , user is infected
                        return true;
                    }
                    userTemp = userTemp->nextMove;
                }
                tempList = tempList->nextMove;
            }
            else {
                tempList = tempList->nextMove;
            }
        }
        patientTemp = patientTemp->nextUser;
    }
    return false;
}

int find_crowded_places(int Day, int TimeStart, int TimeEnd, int Square_Interest[4], int Minimum_duration, multiUserTrajectory* users) {
    int crowd = 0;
    int timer = 0;
    multiUserTrajectory* tempUsers = users;
    
    while (tempUsers != NULL) {
        trajectory* tempPerson = tempUsers->info;
        while (tempPerson != NULL) {
            if (tempPerson->date == Day) {
                if (tempPerson->time_sec >= TimeStart && tempPerson->time_sec <= TimeEnd) {
                    if (tempPerson->xCord >= Square_Interest[0] && tempPerson->xCord <= Square_Interest[2] && tempPerson->yCord >= Square_Interest[1] && tempPerson->yCord <= Square_Interest[3]) {
                        timer += 30;
                    }
                }
                if (timer >= MIN_DURATION) {
                    crowd++;
                    break;
                }
            }
            tempPerson = tempPerson->nextMove;
        }
        tempUsers = tempUsers->nextUser;
    }
    return crowd;
}

//fix gps errors
void repair(int Day, trajectory* userTrajectory) {
    trajectory* first = userTrajectory;
    
    //first and last = a move and the next recorded move
    while (first->date < Day) {
        //cout << "SEARCHING FOR DAY";
        first = first->nextMove;
    }
    
    trajectory* last = first->nextMove;

    /* cout << "first day: " << first->date << "\n";
    cout << "last day: " << last->date << "\n";*/

    while (last != NULL) {
        if (first->date != Day || last->date != Day) {
            break;
        }
        if (last->time_sec - first->time_sec > 30) {
            //if there is an error
            trajectory* fix = new trajectory();
            fix->date = first->date;
            fix->marker = first->marker;
            
            fix->time_sec = first->time_sec + 30;
            //approximate speed for the fix move
            int speed = abs(last->xCord- first->xCord) + abs(last->yCord - first->yCord)/abs(last->time_sec - first->time_sec);
            
            int directionx = last->xCord - first->xCord;
            if (directionx > 0) {
                fix->xCord = first->xCord + speed;
            }
            else if (directionx < 0) {
                fix->xCord = first->xCord - speed;
            }
            else {
                fix->xCord = first->xCord;
            }

            int directiony = last->yCord - first->yCord;
            if (directiony > 0) {
                fix->yCord = first->yCord + speed;
            }
            else if (directiony < 0) {
                fix->yCord = first->yCord - speed;
            }
            else {
                fix->yCord = first->yCord;
            }

            //corrections
            if (fix->xCord < 0) {
                fix->xCord = 0;
            }
            else if (fix->xCord > SIZE) {
                fix->xCord = SIZE-1;
            }

            if (fix->yCord < 0) {
                fix->yCord = 0;
            }
            else if(fix->yCord > SIZE){
                fix->yCord = SIZE-1;
            }


            cout << "first " << first->xCord << " " << first->yCord << " " << first->time_sec << "\n";
            cout << "error fix " << fix->xCord << " " << fix->yCord << " " << fix->time_sec << "\n";
            cout << "last " << last->xCord << " " << last->yCord << " " << last->time_sec << "\n";
            first->nextMove = fix;
            fix->nextMove = last;
            cout << "fixed\n\n";
            first = fix;
            //check if there are more errors between fix and last before moving on
        }
        else {
            //if no error move to next pair
            first = last;
            last = last->nextMove;
        }
    }
}

//clear needless points
void summarize_trajectory(int Day, int days_before, trajectory* userTrajectory) {
    int target = Day - days_before;
    int r = 2;
    int distance;
    trajectory* checkNext;
    trajectory* temp = userTrajectory;
    while (temp != NULL) {
        if (temp->date < target) {
            temp = temp->nextMove;
        }
        else if(temp->date > target){
            break;
        }
        else {
            checkNext = temp->nextMove;
            if (checkNext != NULL) {
                distance = sqrt(pow((temp->xCord - checkNext->xCord), 2) + pow((temp->yCord - checkNext->yCord), 2));
                if ((checkNext->xCord - temp->xCord) + (checkNext->yCord - temp->yCord) <= r) {
                    temp->nextMove = checkNext->nextMove;
                    temp = temp->nextMove;
                    delete(checkNext);
                    checkNext = temp->nextMove;
                }
                else {
                    temp = temp->nextMove;
                    checkNext = checkNext->nextMove;
                }
            } 
        }
    }
    temp = userTrajectory;
    while (temp!=NULL) {
        cout << "Clean x : " << temp->xCord << "Clean y: " << temp->yCord << "\n";
        temp = temp->nextMove;
    }
}

//function to move users around the map
void move_all_users(multiUserTrajectory* userTrajectory, int day, int timer) {
    multiUserTrajectory* tempUser = userTrajectory;
    int speed;
    int direction;
    int delay;
    while (tempUser!= NULL) {
        delay = rand() % 100;
        //chance for a user to delay their movements
        if (delay > 8) {
            //if not delayed find their last move
            trajectory* path = tempUser->info;
            while (path->nextMove != NULL) {
                path = path->nextMove;
                //parses through the list
            }

            trajectory* newpath = new trajectory();
            //pick speed
            speed = 3 + rand() % 3;
            //pick direction
            direction = rand() % 8;
            switch (direction) {
            case 0:
                newpath->yCord = path->yCord - speed;
                break;
            case 1:
                newpath->yCord = path->yCord - speed;
                newpath->xCord = path->xCord + speed;
                break;
            case 2:
                newpath->xCord = path->xCord + speed;
                break;
            case 3:
                newpath->xCord = path->xCord + speed;
                newpath->yCord = path->yCord + speed;
                break;
            case 4:
                newpath->yCord = path->yCord + speed;
                break;
            case 5:
                newpath->yCord = path->yCord + speed;
                newpath->xCord = path->xCord - speed;
                break;
            case 6:
                newpath->xCord = path->xCord - speed;
                break;
            case 7:
                newpath->xCord = path->xCord - speed;
                newpath->yCord = path->yCord - speed;
                break;
            }
            //corrections in case the user tries to move out of bounds
            if (newpath->xCord < 0) {
                //cout << newpath->xCord << " problematic, correcting\n";
                newpath->xCord = 0;
            }
            else if(newpath->xCord > SIZE) {
                //cout << newpath->xCord << " problematic, correcting\n";
                newpath->xCord = SIZE-1;
            }

            if (newpath->yCord < 0) {
                //cout << newpath->yCord << " problematic, correcting\n";
                newpath->yCord = 0;
            }
            else if (newpath->yCord > SIZE) {
                //cout << newpath->yCord << " problematic, correcting\n";
                newpath->yCord = SIZE-1;
            }
            if (day != path->date) {
                //marker points to start of new day
                newpath->date = day;
                newpath->marker = newpath;
                newpath->time_sec = 0;
            }
            else {
                //or marker points to the start of the current day
                newpath->date = path->date;
                newpath->marker = path->marker;
                newpath->time_sec = timer*30;
            }
            //gps fail chance
            int gps_fail_chance = rand() % 100;
            if (gps_fail_chance < 99 ) {
                path->nextMove = newpath;
                newpath->nextMove = NULL;  
            }
            else {
                //cout << "gps fail day:" << day << "\n";
                delete(newpath);
            }
            
        }
        /*else {
            cout << "user is delayed\n";
        }*/
        tempUser = tempUser->nextUser;
    }

}

//initializing the lists of users and patients
multiUserTrajectory* initialize_users(int num_of_users) {
    
    multiUserTrajectory* userTrajectoryPtr = new multiUserTrajectory();
    
    trajectory* userTrajectory = new trajectory();
    userTrajectory->xCord = rand() % SIZE;
    userTrajectory->yCord = rand() % SIZE;
    //random placement in the map
    userTrajectory->time_sec = 0;
    userTrajectory->nextMove = NULL;
    userTrajectory->date = 0;
    userTrajectory->marker = userTrajectory;

    userTrajectoryPtr->info = userTrajectory;
    userTrajectoryPtr->nextUser = NULL;

    multiUserTrajectory* temp = userTrajectoryPtr;   
    
    for (int i = 0; i < num_of_users-1; i++) {
        multiUserTrajectory* temp1 = new multiUserTrajectory();
        userTrajectory = new trajectory();
        userTrajectory->xCord = rand() % SIZE;
        userTrajectory->yCord = rand() % SIZE;
        userTrajectory->time_sec = 0;
        userTrajectory->nextMove = NULL;
        userTrajectory->date = 0;
        userTrajectory->marker = userTrajectory;

        temp1->info = userTrajectory;
        temp1->nextUser = NULL;

        temp->nextUser = temp1;
        temp = temp1;
    }

    return userTrajectoryPtr;
}

int main() {
    //initializing confirmed patients and users
    int timer = 1;
    int userNumber;
    //timer = 1 because the first move is already calculated in intialization
    int day = 0;
    multiUserTrajectory* patientTrajectory = initialize_users(PATIENTS);
    multiUserTrajectory* userTrajectory = initialize_users(USERS);
    
    

    while (true) {
        //move the users around the map "every 30 seconds"
        move_all_users(userTrajectory, day, timer);        
        move_all_users(patientTrajectory, day, timer);

        timer++;
        if (timer % 2280 == 0) {
            userNumber = 0;
            //day change events for non-patient users
            multiUserTrajectory* temp = userTrajectory;
            while(temp != NULL)
            {
                userNumber++;
                trajectory* user = temp->info;
                string response;
                
                cout << "User " << userNumber << ": Do you want to repair the signal? y/n \n";
                cin >> response;
                if (response.compare("y") == 0 || response.compare("Y") == 0) {
                    repair(day, user); //fix any gps errors in the day
                }
                else {
                    cout << "no repair\n";
                }

                cout << "\nUser " << userNumber << ": Do you want to test for possible infection? y/n \n";
                cin >> response;
                if (response.compare("y") == 0 || response.compare("Y") == 0) {
                    if (possible_covid_19_infection(user, day, patientTrajectory)) {
                        cout << "Warning: High chance of infection by covid 19\n";
                    }
                    else {
                        cout << "Low chance of infection by covid 19\n";
                    }
                }
                else {
                    cout << "Didn't test\n";
                }
                
                if (day >= DAYS_BEFORE) {
                    cout << "Summarizing User Trajectory\n";
                    summarize_trajectory(day, DAYS_BEFORE, user);
                }
                temp = temp->nextUser;
            }
            
            if (ADMIN) {
                int crowd = 0;
                crowd += find_crowded_places(day, SEARCH_TIME_START, SEARCH_TIME_END, REGION_OF_INTEREST, MIN_DURATION, userTrajectory);
                crowd += find_crowded_places(day, SEARCH_TIME_START, SEARCH_TIME_END, REGION_OF_INTEREST, MIN_DURATION, patientTrajectory);
                cout << "This many people gathered:" << crowd << "\n";
            }
            day++;
            cout << "\nGoodmorning! Day:" << day << "\n";
            timer = 0;
        }
        if (day > DAYS) {
            cout << "press enter to end\n";
            cin.get();
            return 0;
        }
    }
    return 0;
}
