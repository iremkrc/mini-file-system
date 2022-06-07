İrem Karaca-71879 & Birkan Çelik-68813

## Mini File Manager
All parts of our code is working properly and passing all of the tests. 

## Implementation 
We implemented a file system API library which operates on virtual disks. The file system is based on File Allocation Table(FAT). 

We have never used txt files in the project. 

Virtual Disk can perform all of the spesifications stated in the manuel pdf. 

In mini_fat_save(fs) functions we wrote the file metadata as follows:<br>
<size_of_file_data size_of_filename block_count block_ids file_name><br>
sample file metadata: <2635 9 2 1 2 "file1.txt">

## Run Mini File System locally

### Step 1: clone the project
    git clone https://github.com/iremkrc/mini-file-system.git
    cd mini-file-system
    
### Step 2: compile and run the project
    make
    ./minifs
