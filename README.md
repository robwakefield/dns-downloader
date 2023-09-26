# Get the files
There are 2 ways to get these files. Using Git allows for you to easily receive updates using the `git pull` command. If you do not use `git` then you can download a zipped version of the files.
**THE SOURCE CODE IS LOCATED IN THE DIRECTORY SRC/**

## Cloning via Git
To get the files from this github repository on your machine
- Ensure `git` is installed by running `git -v`
- Navigate to where you would like to download the source files to in terminal
- Run the command `git clone git@github.com:robwakefield/dns-updater-script.git [FOLDER_NAME]` where `FOLDER_NAME` is the name of the folder you want this code to reside in
- This will download these files to the directory `FOLDER_NAME`

## Download ZIP
- From the website `https://github.com/robwakefield/dns-updater-script` click the green `code` button
- On the popup click `Download ZIP`
- Once downloaded, unzip the folder and copy it to where you want it on your file system

# Building
In order to run the files you must first build from the the source code.
You must have gcc installed to build from the source code. (Run command `gcc -v` to verify)
- Navigate to the directory `src/` in terminal
- Run the command `make`
- The executables should now be built and are located in the respective directories `script1/` and `script2/`
- If you are having problems with the script updating, ensure all files are saved, run the command `make clean` and try the above steps again

# Running
To run the files you must first build from the the source code. See section above. You build the files once (or after every change to source code) and then you may run them as many times as you want.

## Manual Run
Example shown to run *script1*. Change to *script2* to run script2.
- Ensure program has been built from source
- Navigate to `src/script1/` in terminal
- Run commmand `sudo ./script1`

## Automatic Run using Cron
To run the program every 10 minutes using cron. Change '10' in the following to change how often the program is run.
- Run the command `sudo crontab -e`
- Add this line to the bottom of the file (script1) `*/10 * * * * sudo cd [Path to src/script1/ folder] && sudo ./script1`
- Then add this line to the bottom of the file (script2) `*/10 * * * * sudo cd [Path to src/script2/ folder] && sudo ./script2`
- Exit from the terminal editor

# Variables

## URLs
The URLs for each script are located in `src/script1/urls.txt` and `src/script2/urls.txt`.
Both files follow the format `FILENAME URL INCREMENT`, where INCREMENT is an optional integer that will be automatically updated every time the file has been downloaded.
To add a new URL, simply add the line `FILENAME URL` to the end of the correct `urls.txt` file

## Time between downloads
See **Automatic Run using Cron** Section

## Directory to save downloaded files
Example shown for *script1*. Change to *script2* for script2.
- Open the file `src/script1/script1.c`
- Near the top of the file will be the line `#define DEST_DIR "/etc/bind/master"`
- Change the directory to update where the downloaded files are stored
- Navigate to `src/` in terminal
- Run command `make` as the source code has been updated and the program needs to be rebuilt

# Logs
Logs are stored in `src/script1-logs.txt` and `src/script2-logs.txt` and are updated every time the program is run.
Any errors encountered by the program will not stop it from running, but will be reported in the logs.
