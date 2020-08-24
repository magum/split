#include <stdio.h>
#include <string.h>
#include <string>

const int chunk_size = 64;

int split(const char * file)
{
    FILE * fp = fopen(file, "rb");
    if (!fp)
    {
        perror("File is not exists");
        return -1;
    }

    unsigned char  buff[chunk_size];
    for (;;)
    {
        memset(buff,0,sizeof(buff));
        size_t read = fread(buff, 1, sizeof(buff), fp);

        if (read > 0)
        {
            for (int i =0; i<sizeof(buff); ++i)
                printf("%02x", buff[i]);
            printf("\n");
        }

        if (read != sizeof(buff))
            break;
    }

    fclose(fp);
    return 0;
}

int concat(const char * file)
{
    FILE * fp = fopen(file, "r");
    if (!fp)
    {
        perror("File is not exists");
        return -1;
    }

    int result = 0;

    char  buff[chunk_size*2+2]; //LF + zero term

    std::string str;

    for (int j=0;j<4;j++)
    {
        memset(buff,0,sizeof(buff));
        const char * read = fgets(buff, sizeof(buff), fp);
        if (read == NULL)
            break;

        if (strlen(buff) != chunk_size*2+1)
        {
            perror("Wrong file format [line size]");
            result = -1;
            break;
        }


        if (feof(fp))
        {
            perror("Wrong file format [eof]");
            result = -1;
            break;
        }

        for (int i =0; i<chunk_size*2; i+=2)
        {
            int ch = 0;
            char chStr[3] = {0};
            chStr[0] = buff[i];
            chStr[1] = buff[i+1];
            sscanf(chStr, "%02x", &ch);
            // printf("%s %02x\n", chStr, ch);
            str += (char)ch;
        }
    }

    printf("size %d\n", str.size());

    fclose(fp);

    fp = fopen("out", "w");
    if (!fp)
    {
        perror("Failed to open file to write");
        return -1;
    }

    fwrite(str.c_str(), str.size(), 1, fp);
    fclose(fp);

    return result;
}

int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        perror("Args expected");
        return -1;
    }

    const char * cmd = argv[1];
    const char * file = argv[2];

    // printf("cmd %s, file %s\n", cmd, file);

    if (!strcmp(cmd, "split"))
    {
        return split(file);
    }
    else if (!strcmp(cmd, "concat"))
    {
        return concat(file);
    }
    else
    {
        perror("Wrong cmd");
        return -1;
    }

    return 0;
}
