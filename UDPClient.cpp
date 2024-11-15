#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

// allows for "either host name or IP address to be specified on the command line" as per the directions
// for example you can use the host-name localhost or the IP address that correlates to local host which is 127.0.0.1
int main(int argc, char const *argv[])
{

    // convert args to ints as well for error handling...
    if (argc != 5 || stoi(argv[2]) < 0 || stoi(argv[3]) < 0 || stoi(argv[4]) < 0)
    {
        cout << "Error, please input the following command line arguments in the following order: "
             << "host-name/IP address of the server, given loan amount, loan period, annual interest rate" << endl;
        // we want to return 0 since we assume the server will keep running, but in the directions it was specified
        // that we want user to enter args on CML, hence - the client side will have to enter again (even if server is still running)
        return 0;
    }

    // variables:
    //==========================================================
    double monthly_paym = 0;
    double total_paym = 0;

    int sockfd;
    int n = 0;

    // string that will contain contents sent from server
    string buffer_2;
    string info_str;

    char buffer[1024];

    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    //==========================================================

    // put string together with the required CML args, separate by space
    info_str = string(argv[2]) + " " + string(argv[3]) + " " + string(argv[4]);

    // creating UDP socket ...
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // initialize struct...
    memset(&serv_addr, 0, len);

    // specify ipv4, accept any connection, and properly set port # of serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    // shall be given loan amount ---> loan period ---> annual interest rate

    // sends the given data (info_str string that was built from CML args) to server address
    n = sendto(sockfd, info_str.c_str(), info_str.length(), 0, (struct sockaddr *)&serv_addr, len);

    // essentially read what server sent basically same func as read() from tcp client side
    n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&serv_addr, &len);

    // now, we append what was read and stored to buffer to buffer_2
    // we append the # of chars that was read, hence forming our string that
    // was otherwise null before
    buffer_2.append(buffer, n);

    // buffer_2 string contains the required info from server... now, we shall derive the info from it
    // to the given variables... use sscanf to parse and convert buffer_2 to c string format
    // to allow that to occur. Now, we can correctly place the retrieved info that was calculated
    // from the server into the given vars. We know monthly pam and total paym
    // to be separated by a space (from our udp server code)... hence, we scan it like that
    // while noting our double vars for accuracy
    sscanf(buffer_2.c_str(), "%lf %lf", &monthly_paym, &total_paym);

    buffer_2.push_back('\0');
    info_str.push_back('\0');
    // as per the directions : "The only output of the program should be the numeric output or an error message if it fails."

    // formatted output as such:
    // loan
    // monthly payment
    // total payment

    // assumming there to actually be a payment, we make it so it must be greater than 0
    if (monthly_paym > 0 && total_paym > 0)
    {
        cout << "\t\t\t\t" << argv[2] << endl
             << "\t\t\t\t" << monthly_paym << endl
             << "\t\t\t\t" << total_paym << endl;
    }
    else
    {
        cout << "error, try again please." << endl;
    }

    close(sockfd);

    return 0;
}
