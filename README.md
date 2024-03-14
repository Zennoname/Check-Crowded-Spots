Check-Crowded-Spots
A government assistance tool for managing the Covid outbreak

The goal of this project was to implement some of the basic functionalities of the Israel Ministry of Health application, as well as additional features to address the Covid-19 pandemic. The functionalities are as follows:

POSSIBLE_COVID_19_INFECTION
This function takes as arguments the list of patients, the user's movement list, and the day we are checking. It then identifies the user's steps within a maximum number of minutes (max_time) from each patient's step and compares them. If the two steps are close to each other, it keeps track of the continuous time the user stays close, and if it exceeds a minimum time (min_time), it is considered to have a high probability of infection.

FIND_CROWDED_PLACES
This function takes as arguments the day, start and end hours of search, as well as coordinates to look into. It also takes the minimum duration, in seconds, for someone to be considered staying in the area, and the list of users. For each user, it accesses their list of steps and records the time they pass through the Square_Interest area. If this time exceeds MIN_DURATION, it increases the count of people gathered.

REPAIR
This function takes as arguments the day we are correcting and the user's movement list. To find GPS problems, the algorithm sets a pointer to the immediately next user's step (last) and keeps one to the previous. As it traverses the list, it checks if the pair of movements has a time difference greater than 30 seconds. If so, it creates a new node (fix) and adds it between the first and last. Then it sets fix = first and leaves last where it was to consider cases of errors greater than one movement.

SUMMARIZE_TRAJECTORY
This function takes as arguments the current day, the constant indicating how many days back we summarize, and the list of user steps. Then it reaches the desired day and looks at the movements. If two movements are very close to each other, the second one is deleted, and the algorithm continues until the day ends.

INITIALIZE_USERS
This function takes a number of users and initializes a list with pointers to the head of each user's movement list.

MOVE_ALL_USERS
This function takes the list of users and based on the Random Waypoint model decides on user movements, adding each new movement to the end of the movement list. Within this function, speed, delay probability, and GPS failure probability are calculated.

Finally, two different structs were created for code execution. The trajectory is the struct used for the nodes of the user's movement list and contains all necessary information along with auxiliary pointers for changing the day. The multiuser Trajectory is the struct used for the lists of users and has pointers to the beginning of each movement list for each user.
