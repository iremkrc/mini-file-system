#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include <cassert>
#include <stdarg.h>

#include <list>

#include "fat.h"
#include "fat_file.h"



/**
 * Write inside one block in the filesystem.
 * @param  fs           filesystem
 * @param  block_id     index of block in the filesystem
 * @param  block_offset offset inside the block
 * @param  size         size to write, must be less than BLOCK_SIZE
 * @param  buffer       data buffer
 * @return              written byte count
 */
int mini_fat_write_in_block(FAT_FILESYSTEM *fs, const int block_id, const int block_offset, const int size, const void * buffer) {
	assert(block_offset >= 0);
	assert(block_offset < fs->block_size);
	assert(size + block_offset <= fs->block_size);

	int written = 0;

	// TODO: write in the real file.

	return written;
}

/**
 * Read inside one block in the filesystem
 * @param  fs           filesystem
 * @param  block_id     index of block in the filesystem
 * @param  block_offset offset inside the block
 * @param  size         size to read, must fit inside the block
 * @param  buffer       buffer to write the read stuff to
 * @return              read byte count
 */
int mini_fat_read_in_block(FAT_FILESYSTEM *fs, const int block_id, const int block_offset, const int size, void * buffer) {
	assert(block_offset >= 0);
	assert(block_offset < fs->block_size);
	assert(size + block_offset <= fs->block_size);

	int read = 0;

	// TODO: read from the real file.

	return read;
}


/**
 * Find the first empty block in filesystem.
 * @return -1 on failure, index of block on success
 */
int mini_fat_find_empty_block(const FAT_FILESYSTEM *fat) {
	// TODO: find an empty block in fat and return its index.
	for(int i = 0; i < fat->block_count; i++) {
		if(fat->block_map[i] == 0) {
			return i;
		}
	}
	return -1;
}

/**
 * Find the first empty block in filesystem, and allocate it to a type,
 * i.e., set block_map[new_block_index] to the specified type.
 * @return -1 on failure, new_block_index on success
 */
int mini_fat_allocate_new_block(FAT_FILESYSTEM *fs, const unsigned char block_type) {
	int new_block_index = mini_fat_find_empty_block(fs);
	if (new_block_index == -1)
	{
		return -1;
	}
	fs->block_map[new_block_index] = block_type;
	return new_block_index;
}

void mini_fat_dump(const FAT_FILESYSTEM *fat) {
	printf("Dumping fat with %d blocks of size %d:\n", fat->block_count, fat->block_size);
	for (int i=0; i<fat->block_count;++i) {
		printf("%d ", (int)fat->block_map[i]);
	}
	printf("\n");

	for (int i=0; i<fat->files.size(); ++i) {
		mini_file_dump(fat, fat->files[i]);
	}
}

static FAT_FILESYSTEM * mini_fat_create_internal(const char * filename, const int block_size, const int block_count) {
	FAT_FILESYSTEM * fat = new FAT_FILESYSTEM;
	fat->filename = filename;
	fat->block_size = block_size;
	fat->block_count = block_count;
	fat->block_map.resize(fat->block_count, EMPTY_BLOCK); // Set all blocks to empty.
	fat->block_map[0] = METADATA_BLOCK;
	
	return fat;
}

/**
 * Create a new virtual disk file.
 * The file should be of the exact size block_size * block_count bytes.
 * Overwrites existing files. Resizes block_map to block_count size.
 * @param  filename    name of the file on real disk
 * @param  block_size  size of each block
 * @param  block_count number of blocks
 * @return             FAT_FILESYSTEM pointer with parameters set.
 */
FAT_FILESYSTEM * mini_fat_create(const char * filename, const int block_size, const int block_count) {

	FAT_FILESYSTEM * fat = mini_fat_create_internal(filename, block_size, block_count);

	// TODO: create the corresponding virtual disk file with appropriate size.

	int disk_size = block_size * block_count;
	FILE* virtual_disk = fopen(filename, "w");
	fseek(virtual_disk, disk_size, SEEK_SET);
	//fwrite(fat, 1, sizeof(fat), virtual_harddisk);
	fputc('\0', virtual_disk);
	fclose(virtual_disk);

	return fat;
}

/**
 * Save a virtual disk (filesystem) to file on real disk.
 * Stores filesystem metadata (e.g., block_size, block_count, block_map, etc.)
 * in block 0.
 * Stores file metadata (name, size, block map) in their corresponding blocks.
 * Does not store file data (they are written directly via write API).
 * @param  fat virtual disk filesystem
 * @return     true on success
 */
bool mini_fat_save(const FAT_FILESYSTEM *fat) {
    FILE * fat_fd = fopen(fat->filename, "r+");
    if (fat_fd == NULL) {
        perror("Cannot save fat to file");
        return false;
    }
    // TODO: save all metadata (filesystem metadata, file metadata).
    int number_of_blocks = fat->block_count;
    int size = fat->block_size;
    fseek(fat_fd, 0, SEEK_SET);
    fwrite(&number_of_blocks, sizeof(number_of_blocks), 1, fat_fd);
    fwrite(&size, sizeof(size), 1, fat_fd);
    


    for(int i : fat->block_map){
        fwrite(&i, sizeof(i), 1, fat_fd);
    }

    

    for(int i = 0; i < fat->files.size(); i++){
        int metadata_id = fat->files[i]->metadata_block_id;
        
        fseek(fat_fd, metadata_id * size, SEEK_SET);
        fwrite(&(fat->files[i]->size), sizeof(fat->files[i]->size), 1, fat_fd);
        

        int length = strlen(fat->files[i]->name);
        fwrite(&length, sizeof(length), 1, fat_fd);
        int block_id_count = fat->files[i]->block_ids.size();
        fwrite(&block_id_count, sizeof(block_id_count), 1, fat_fd); // write length of block ids to be able read in load 
        
        for(int j : fat->files[i]->block_ids){
            fwrite(&j, sizeof(j), 1, fat_fd);
        }
        fwrite(fat->files[i]->name, sizeof(fat->files[i]->name), 1, fat_fd);
    }
    fclose(fat_fd);
    return true;
}

FAT_FILESYSTEM * mini_fat_load(const char *filename) {
    FILE * fat_fd = fopen(filename, "r+");
    if (fat_fd == NULL) {
        perror("Cannot load fat from file");
        exit(-1);
    }
    // TODO: load all metadata (filesystem metadata, file metadata) and create filesystem.

    int block_size = 1024, block_count = 10;
    FAT_FILESYSTEM * fat = mini_fat_create_internal(filename, block_size, block_count);
    int read;
    fseek(fat_fd, 0, SEEK_SET);
    fread(&read, 1, sizeof(int), fat_fd); // block_count
    fread(&read, 1, sizeof(int), fat_fd); // block_size

    for(int i=0;i< block_count;i++){
        fread(&read, 1, sizeof(int), fat_fd); // block_map
        fat->block_map[i] = read;
    }
    int file_count =0;
    for(int i =0;i<fat->block_count;i++){
        if(fat->block_map[i]==FILE_ENTRY_BLOCK){
            fseek(fat_fd, i * block_size, SEEK_SET);
            fread(&read, 1, sizeof(int), fat_fd); // file size
            int file_size = read;
            
            fread(&read, 1, sizeof(int), fat_fd); // file name length
            int file_name_length = read;

            fread(&read, 1, sizeof(int), fat_fd); // block_id_count
            int temp_block_id_count = read;

            std::vector<int> block_ids_info;

            for(int j=0;j<temp_block_id_count;j++){
                fread(&read, 1, sizeof(int), fat_fd); // block_id
                block_ids_info.push_back(read);
            }

            char file_name[file_name_length+1];
            fread(file_name, 1, sizeof(file_name), fat_fd); // file name
            file_name[file_name_length] = '\0';

            FAT_FILE *temp = mini_file_create(file_name);       
            temp->size = file_size; 
            fat->files.push_back(temp);

            fat->files[file_count]->metadata_block_id = block_ids_info[0];
            for(int k: block_ids_info){
                fat->files[file_count]->block_ids.push_back(k);
            }
            
            file_count++;
        }
    }
    return fat;
}
