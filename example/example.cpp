#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <lidarlite.h>

int getkey() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

int main() {
    LidarLite *lidarLite = new LidarLite() ;
    int err = lidarLite->openLidarLite();
    if (err < 0){
        printf("Error: %d", lidarLite->error);
    } else {

        int hardwareVersion = lidarLite->getHardwareVersion() ;
        int softwareVersion = lidarLite->getSoftwareVersion() ;
        printf("Hardware Version: %d\n",hardwareVersion) ;
        printf("Software Version: %d\n",softwareVersion) ;

        // 27 is the ESC key

        while(lidarLite->error >= 0 && getkey() != 27){
            int distance = lidarLite->getDistance();
            if (distance < 0) {
                int llError ;
                llError = lidarLite->getError() ;
                printf("Lidar-Lite error: %d\n",llError) ;
            } else {
                int previousDistance = lidarLite->getPreviousDistance();
                // printf("Distance: %dcm\n", dist);
                int velocity = lidarLite->getVelocity();
                printf("Distance: %5d cm  |  Previous Distance: %5d cm   | Velocity: % 8d \n",distance,previousDistance,velocity);
            }
        }
    }
    lidarLite->closeLidarLite();
}
