İrem Karaca-0071879 & Birkan Çelik-0068813

## Mini File Manager
All parts of our code is working properly and passing all of the tests. 

## Implementation 
We implemented a file system API library which operates on virtual disks. The file system is based on File Allocation Table(FAT). 

We have never used txt files in the project. 

Virtual Disk can perform all of the spesifications stated in the manuel pdf. 

In mini_fat_save(fs) functions we wrote the file metadata as follows: 
<size_of_file_data size_of_filename block_count block_ids file_name>
sample file metadata: <2635 9 2 1 2 "file1.txt">
