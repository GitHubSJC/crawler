#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
// #include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

class Url
{
public:
    void EntryUrl()
    {
        cout << "entry url: ";
        cin >> input;
    }

    const char *GetHead(string imgurl)
    {
        if(strstr(imgurl.c_str(), "http") != NULL)
        {
            int cut = imgurl.find('/');
            string cuthttp(imgurl, cut + 2);
            int flag = cuthttp.find('/');
            if(flag == -1)
            {
                resource = "/";
            }
            else
            {
                string tmp1(cuthttp, flag);
                resource = tmp1;
            }
            if(strstr(cuthttp.c_str(), "com") != NULL || strstr(cuthttp.c_str(), "cn") != NULL || strstr(cuthttp.c_str(), "net") != NULL)
            {
                string tmp2(cuthttp.c_str(), flag);
                host = tmp2;
            }
            else
            {
                host = consthost;
            }
        }
        else
        {
            string cuthttp(imgurl);
            int flag = cuthttp.find('/');
            if(flag == -1)
            {
                resource = "/";
            }
            else
            {
                string tmp1(cuthttp, flag);
                resource = tmp1;
            }
            if(strstr(cuthttp.c_str(), "com") != NULL || strstr(cuthttp.c_str(), "cn") != NULL || strstr(cuthttp.c_str(), "net") != NULL)
            {
                host = cuthttp;
            }
            else
            {
                host = consthost;
            }
        }
        http.clear();
        http = "GET " + resource + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
        cout << http << endl;
        return http.c_str();
    }

    const char *GetHead()
    {
        return http.c_str();
    }

    string GetInput()
    {
        return input;
    }

    char *GetIp()
    {
        struct hostent *ht = gethostbyname(host.c_str());
        if(ht == NULL)
        {
            cout << "no internet" << endl;
            exit(1);
        }
        cout << inet_ntoa(*((struct in_addr *)ht->h_addr)) << endl;
        return inet_ntoa(*((struct in_addr *)ht->h_addr));
    }

    char *GetImgIp()
    {
        struct hostent *ht = gethostbyname(http.c_str());
        if(ht == NULL)
        {
            cout << "no internet" << endl;
            exit(1);
        }
        cout << inet_ntoa(*((struct in_addr *)ht->h_addr)) << endl;
        return inet_ntoa(*((struct in_addr *)ht->h_addr));
    }

    ssize_t GetLength()
    {
        return http.length();
    }

    string GetHost()
    {
        return host;
    }

    void SaveHost(string curhost)
    {
        consthost = curhost;
    }

private:
    string input;
    string resource;
    string host;
    string consthost;
    string http;
};

class Transmission
{
public:
    Transmission()
    {
        sockfd = -1;
        at = 1;
    }

    void Connect(char *ip)
    {
        sockfd = socket(PF_INET, SOCK_STREAM, 0);
        if(sockfd == -1)
        {
            cerr << "create sock filed" << endl;
            exit(1);
        }
        struct sockaddr_in ser;
        ser.sin_family = AF_INET;
        ser.sin_port = htons(80);
        ser.sin_addr.s_addr = inet_addr(ip);

        int con = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));
        if(con == -1)
        {
            cerr << "connect filed" << endl;
            exit(1);
        }
        else
        {
            cout << "connect suc" << endl;
        }
    }

    void Send(const char *buf, ssize_t length)
    {
        ssize_t sen = send(sockfd, buf, length, 0);
        if(sen == -1)
        {
            cerr << "send filed" << endl;
        }
        else
        {
            cout << "send suc" << endl;
        }
    }

    void Send(string imgurl)
    {
        at = 1;
        if(strstr(imgurl.c_str(), "png"))
        {
            at = 2;
        }
        if(strstr(imgurl.c_str(), "gif"))
        {
            at = 3;
        }
        ssize_t sen = send(sockfd, imgurl.c_str(), imgurl.size(), 0);
        if(sen == -1)
        {
            cerr << "send filed" << endl;
        }
        else
        {
            cout << "send suc" << endl;
        }
    }

    void Receive()
    {
        char buf[1024] = {0};
        FILE *fp = fopen("/root/code/爬虫/http.txt", "wb+");
        while(1)
        {
            ssize_t rec = recv(sockfd, buf, sizeof(buf), 0);
            cout << rec << endl;
            if(rec <= 0)
            {
                break;
            }
            fwrite(buf, rec, 1, fp);
        }
        cout << "over" << endl;
        close(sockfd);
        fclose(fp);
    }

    void ImgReceive(int i)
    {
        char pathname[50] = "/root/code/爬虫/image/";
        if(i < 10)
        {
            char filename[6] = "1.jpg";
            if(at == 2)
            {
                filename[2] = 'p';
                filename[3] = 'n';
                filename[4] = 'g';
            }
            else if(at == 3)
            {
                filename[2] = 'g';
                filename[3] = 'i';
                filename[4] = 'f';
            }
            filename[0] = '0' + i;
            strcat(pathname, filename);
        }
        else if(i >= 10 && i < 100)
        {
            char filename[7] = "10.jpg";
            if(at == 2)
            {
                filename[3] = 'p';
                filename[4] = 'n';
                filename[5] = 'g';
            }
            else if(at == 3)
            {
                filename[3] = 'g';
                filename[4] = 'i';
                filename[5] = 'f';
            }
            filename[0] = '0' + i / 10;
            filename[1] = '0' + i % 10;
            strcat(pathname, filename);
        }
        // else if(i >= 100)
        // {
        //     char filename[8] = "100.jpg";
        //     filename[0] = '0' + i;
        //     strcat(pathname, filename);
        // }
        cout << pathname << endl;
        char buf[1024] = {0};
        FILE *pp = fopen(pathname, "wb+");
        int flag = 0;
        char *p = NULL;
        while(1)
        {
            ssize_t rec = recv(sockfd, buf, sizeof(buf), 0);
            cout << rec << endl;
            if(rec <= 0)
            {
                break;
            }
            if(flag == 0)
            {
                if((p = strstr(buf, "\r\n\r\n")) != NULL)
                {
                    fwrite(buf + (p - buf) + 4, rec - (p - buf) - 4, 1, pp);
                    flag = 1;
                }
                else
                {
                    fwrite(buf, rec, 1, pp);
                    break;
                }
            }
            else
            {
                fwrite(buf, rec, 1, pp);
            }
        }
        cout << "over" << endl;
        close(sockfd);
        fclose(pp);
    }

private:
    int sockfd;
    int at;
};

class HandleData
{
public:
    void DownloadUrl()
    {
        char line[2048];
        FILE *fp = fopen("/root/code/爬虫/http.txt", "r+");
        while(fgets(line, 2048, fp) != NULL)
        {
            char *pi = line;
            if(*++pi == '<' && *++pi == 'a' && *++pi == ' ' && *++pi == 'h' && *++pi == 'r' && *++pi == 'e' && *++pi == 'f')
            {
                pi++;
                char *pbegin = pi;
                int count = 0;
                while(*pi != '"')
                {
                    count++;
                    pi++;
                }
                string tmp(pbegin, count);
                url.push_back(tmp);
            }
        }
    }

    void DownloadImg()
    {
        char line[2048];
        FILE *fp = fopen("/root/code/爬虫/http.txt", "r+");
        while(fgets(line, 2048, fp) != NULL)
        {
            char *pi = line;
            int fflag = 0;
            while(*pi != '\0')
            {
                if(fflag == 0)
                {
                    if(*++pi == '<')
                    {
                        if(*++pi == 'i' && *++pi == 'm' && *++pi == 'g')
                        {
                            fflag = 1;
                        }
                        if(*pi == 'I' && *++pi == 'M' && *++pi == 'G')
                        {
                            fflag = 1;
                        }
                    }
                }
                if(fflag == 1)
                {
                    if(*++pi == 's' && *++pi == 'r' && *++pi == 'c' && *++pi == '=' && *++pi == '"')
                    {
                        pi++;
                        char *pbegin = pi;
                        int count = 0;
                        fflag = 0;
                        while(*pi != '"')
                        {
                            count++;
                            pi++;
                        }
                        string tmp(pbegin, count);
                        imgurl.push_back(tmp);
                    }
                }
            }
        }
        fclose(fp);
        cout << "一共有" << imgurl.size() << endl;
    }

    vector<string> GetImgUrl()
    {
        return imgurl;
    }

    int GetImgNum()
    {
        return imgurl.size();
    }

private:
    vector<string> imgurl;
    vector<string> url;
};

int main()
{
    Url url;
    Transmission t;
    HandleData h;

    url.EntryUrl();
    url.GetHead(url.GetInput());
    url.SaveHost(url.GetHost());

    t.Connect(url.GetIp());
    t.Send(url.GetHead(url.GetInput()), url.GetLength());
    t.Receive();

    h.DownloadImg();
    for(int i = 0; i < h.GetImgNum(); i++)
    {
        url.GetHead(h.GetImgUrl()[i]);
        t.Connect(url.GetIp());
        t.Send(url.GetHead());
        t.ImgReceive(i + 1);
        cout << "done!" << endl;
        cout << "" << endl;
    }

    return 0;
}
