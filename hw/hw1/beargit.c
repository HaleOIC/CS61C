#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

#define MAX_COMMIT_NUMBER 2048
/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE* fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");


  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE* fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  int flag = 0;
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");

    if (strcmp(line, filename) == 0) {
      flag = 1;
      continue;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  if (flag == 0) {
    fprintf(stderr, "ERROR: File %s not tracked\n", filename);
    fclose(findex);
    fclose(fnewindex);
    fs_rm(".beargit/.newindex");
    return 1;
  } else {
    fclose(findex);
    fclose(fnewindex);
    fs_mv(".beargit/.newindex", ".beargit/.index");
    return 0;
  }
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  if (strstr(msg, go_bears) == NULL) {
    return 0;
  } else {
    return 1;
  }
}

void next_commit_id(char* commit_id) {
  char* new_id = commit_id;
  int i;
  for (i = 0; i < COMMIT_ID_BYTES; i++) {
    if (*new_id == '0') {
      *new_id = '6';
      new_id++;
      continue;
    } else if (*new_id == '6') {
      *new_id = '1';
      break;
    } else if (*new_id == '1') {
      *new_id = 'c';
      break;
    } else if (*new_id == 'c') {
      *new_id = '6';
      new_id++;
    } else {
      *new_id = '6';
      new_id++;
    }
  }
  commit_id[COMMIT_ID_BYTES] = '\0';
}

int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  //generate a new directory and copy the targer files
  char dirname[FILENAME_SIZE]; strcpy(dirname, ".beargit/");
  strcat(dirname, commit_id);
  fs_mkdir(dirname); 
  char new_index[FILENAME_SIZE], new_prev[FILENAME_SIZE];
  strcpy(new_index, dirname); strcpy(new_prev, dirname);
  strcat(new_index, "/.index"); strcat(new_prev, "/.prev");

  fs_cp(".beargit/.index", new_index);
  fs_cp(".beargit/.prev", new_prev);

  // copy all the tracked files
  FILE* findex = fopen(".beargit/.index", "r");
  char filename[FILENAME_SIZE];
  while (fgets(filename, sizeof(filename), findex)) {
    strtok(filename, "\n");
    char new_file[FILENAME_SIZE]; strcpy(new_file, dirname);
    strcat(new_file, "/"); strcat(new_file, filename);
    fs_cp(filename, new_file);
  }

  // write the msg to the msg file.
  char msgfile[MSG_SIZE]; strcpy(msgfile, dirname);
  strcat(msgfile, "/.msg");
  FILE* msgptr = fopen(msgfile, "w");
  fprintf(msgptr, "%s\n", msg);
  fclose(msgptr);

  // write the new ID into the .beargit/.prev
  FILE* prev = fopen(".beargit/.prev", "w");
  fprintf(prev, "%s", commit_id);
  fclose(prev);

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  
  FILE* findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];
  fprintf(stdout, "Tracked files:\n\n");
  int tot_amout = 0;
  while (fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fprintf(stdout, "  %s\n", line);
    tot_amout += 1;
  }
  fprintf(stdout, "\n%d files total\n", tot_amout);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* consider a recurisive process*/
  int tot = 0;
  char all_commit[MAX_COMMIT_NUMBER][COMMIT_ID_SIZE];
  char all_msg[MAX_COMMIT_NUMBER][MSG_SIZE];
  char target_prev_pos[FILENAME_SIZE] = ".beargit/.prev";
  char lst_commit[COMMIT_ID_SIZE];
  read_string_from_file(target_prev_pos, lst_commit, COMMIT_ID_BYTES);
  lst_commit[COMMIT_ID_BYTES] = '\0';
  while (strcmp(lst_commit, "0000000000000000000000000000000000000000\0") != 0) {
    strcpy(target_prev_pos, ".beargit/");
    strcat(target_prev_pos, lst_commit);
    char target_msg_pos[FILENAME_SIZE];
    strcpy(target_msg_pos, target_prev_pos);
    strcat(target_prev_pos, "/.prev");
    strcat(target_msg_pos, "/.msg");

    strcpy(all_commit[tot], lst_commit);
    read_string_from_file(target_msg_pos, all_msg[tot++], MSG_SIZE);
    read_string_from_file(target_prev_pos, lst_commit, COMMIT_ID_BYTES);
    lst_commit[COMMIT_ID_BYTES] = '\0';
  }
  if (tot == 0) {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  } else {
    for (int i = 0; i < tot; i++) {
      fprintf(stdout, "\ncommit %s\n    %s", all_commit[i], all_msg[i]);
    }
    fprintf(stdout, "\n");
  }
  return 0;
}
