#include <stdio.h>
#include "util.h"
#include <string.h>

#define MAX_COL 30
#define MAX_REL 5
#define MAX_VAL 30

/* initialize database */
t_db* INIT_DB(char *name) {
	t_db* h;
	h = (t_db *)calloc(1,sizeof(t_db));
	if (!h) return NULL;
	h->tables = NULL;
	strcpy(h->name, name);
	return h;
}

/* search for table "name" and return a pointer to it */
t_table *searchTable(t_db *db, char *name) {
	char *quote = "\"";
	if (!db->tables) {
		printf("Table %s%s%s not found in database.\n", quote, name, quote);
		return NULL;
	}
	t_table *p = db->tables;
	//keep going as long as not found
	while (p != NULL && strcmp(p->name,name) != 0) p = p->next;
	if (p == NULL) {
		printf("Table %s%s%s not found in database.\n", quote, name, quote);
		return NULL;
	}
	return p;
}

/* insert table "name" in database */
void insTable(t_db *db, char *name, char *type, char **columns, int *nr) {
	char *quote = "\"";
	if (strcmp(type,"STRING") && strcmp(type,"INT") && strcmp(type,"FLOAT")) {
		printf("Unknown data type: %s%s%s.\n", quote, type, quote);
		return;
	}
	int i;
	t_table *p, *ant;
	//if it doesn't exist, initialize it
	if (!db->tables) {
		db->tables = (t_table *)calloc(1,sizeof(t_table));
		p = db->tables;
	} else {
		p = db->tables;
		while (p) {
			ant=p;
			if (strcmp(name,p->name) == 0) {
				printf("Table %s%s%s already exists.\n", quote, name, quote);
				return;
			}
			p = p->next;
		}
		p = (t_table *)calloc(1,sizeof(t_table));
		ant->next = p;
	}
	//datatype of table's lines
	strcpy(p->name, name);	//name
	if (strstr(type,"STRING")) {
		p->type = STRING;
		p->lines = NULL;
	} else if (strstr(type,"INT")) {
		p->type = INT;
		p->lines = NULL;
	} else if (strstr(type,"FLOAT")) {
		p->type = FLOAT;
		p->lines = NULL;
	}
	//values of columns
	t_column *col = NULL, *ant_col = NULL;
	for (i=0; i<(*nr); i++) {
		if (i == 0) {
			p->columns = (t_column *)calloc(1,sizeof(t_column));
			col = p->columns;
		} else {
			col = p->columns;
			while (col) ant_col = col, col = col->next;
			col = (t_column *)calloc(1,sizeof(t_column));
			ant_col->next = col;
		}
		strcpy(col->name,columns[i]);
	}
}

/* delete table "name" from database and free all the memory occupied by it */
void DELETE_TABLE(t_db *db, char *name) {
	char *quote = "\"";
	//check if given table exists in database and if so retain its position and previous
	if (!db->tables) {
		printf("Table %s%s%s not found in database.\n", quote, name, quote);
		return;
	}
	t_table *p = db->tables;
	t_table *ant = NULL;
	while (p != NULL && strcmp(p->name,name) != 0) ant = p, p = p->next;
	if (p == NULL) {
		printf("Table %s%s\" not found in database.\n", quote, name);
		return;
	}
	//free the lines and the elements based of their datatype
	if (p->type == 2) {			//STRING
		if (p->lines == NULL) free(p->lines);
		else {
			t_stringLine *line, *nextLine;
			t_stringCell *cell, *nextCell;
			//go through lines and free elements
			for (line = p->lines; line != NULL; line = line->next) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell->value);
					free(cell);
					cell = nextCell;
				}
			}
			//free lines
			line = p->lines;
			while (line) {
				nextLine = line->next;
				free(line);
				line = nextLine;
			}
		}
	} else if (p->type == 0) {			//INT
		if (p->lines == NULL) free(p->lines);
		else {
			t_intLine *line, *nextLine;
			t_intCell *cell, *nextCell;
			for (line = p->lines; line != NULL; line = line->next) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell);
					cell = nextCell;
				}
			}
			line = p->lines;
			while (line) {
				nextLine = line->next;
				free(line);
				line = nextLine;
			}
		}
	} else if (p->type == 1) {			//FLOAT
		if (p->lines == NULL) free(p->lines);
		else {
			t_floatLine *line, *nextLine;
			t_floatCell *cell, *nextCell;
			for (line = p->lines; line != NULL; line = line->next) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell);
					cell = nextCell;
				}
			}
			line = p->lines;
			while (line) {
				nextLine = line->next;
				free(line);
				line = nextLine;
			}
		}
	}
	//free columns
	if (!p->columns) free(p->columns);
	else {
		t_column *col = p->columns;
		t_column *nextCol;
		while (col) {
			nextCol = col->next;
			free(col);
			col = nextCol;
		}
	}
	//free table
	if (ant == NULL) db->tables = p->next; //first table
	else if (p->next == NULL) ant->next = NULL; //last table
	else ant->next = p->next;
	free(p);
}

/* add line in table "name" */
void ADD(t_db *db, char *name, char **values, int *nr_col) {
	//check if given table exists in database and if so retain its position
	t_table *p = searchTable(db, name);
	if (!p) return;
	int i;

	if (p->type == 2) {			//STRING
		t_stringLine *line, *ant;
		t_stringCell *ant_cell;
		//if no lines
		if (!p->lines) {
			p->lines = (t_stringLine *)calloc(1,sizeof(t_stringLine));
			line = p->lines;
			line->cells = NULL;
		} else { //lines remaining
			line = p->lines;
			while (line) ant = line, line = line->next;
			line = (t_stringLine *)calloc(1,sizeof(t_stringLine));
			if (ant) ant->next = line;
			line->cells = NULL;
		}
		t_stringCell *cell;
		//found line and now add elements
		for (i = 0; i < (*nr_col); i++) {
			//at first it's empty so initializing it
			if (!line->cells) {
				line->cells = (t_stringCell *)calloc(1,sizeof(t_stringCell));
				cell = line->cells;
			} else { //if not empty then add elements at the end
				ant_cell = cell;
				cell = (t_stringCell *)calloc(1,sizeof(t_stringCell));
				ant_cell->next = cell;
			}
			cell->value = malloc(MAX_VAL);
			strcpy(cell->value, values[i]);
		}
	} else if (p->type == 0) {		//INT
		t_intLine *line, *ant;
		t_intCell *ant_cell;
		if (!p->lines) {
			p->lines = (t_intLine *)calloc(1,sizeof(t_intLine));
			line = p->lines;
		} else {
			line = p->lines;
			while (line) ant = line, line = line->next;
			line = (t_intLine *)calloc(1,sizeof(t_intLine));
			ant->next = line;
		}
		t_intCell *cell;
		for (i = 0; i < (*nr_col); i++) {
			if (!line->cells) {
				line->cells = (t_intCell *)calloc(1,sizeof(t_intCell));
				cell = line->cells;
			} else {
				ant_cell = cell;
				cell = (t_intCell *)calloc(1,sizeof(t_intCell));
				ant_cell->next = cell;
			}
			cell->value = atoi(values[i]);
		}
	} else if (p->type == 1) {			//FLOAT
		t_floatLine *line, *ant;
		t_floatCell *ant_cell;
		if (!p->lines) {
			p->lines = (t_floatLine *)calloc(1,sizeof(t_floatLine));
			line = p->lines;
		} else {
			line = p->lines;
			while (line) ant = line, line = line->next;
			line = (t_floatLine *)calloc(1,sizeof(t_floatLine));
			ant->next = line;
		}
		t_floatCell *cell;
		for (i = 0; i < (*nr_col); i++) {
			if (!line->cells) {
				line->cells = (t_floatCell *)calloc(1,sizeof(t_floatCell));
				cell = line->cells;
			} else {
				ant_cell = cell;
				cell = (t_floatCell *)calloc(1,sizeof(t_floatCell));
				ant_cell->next = cell;
			}
			cell->value = strtod(values[i], NULL);
		}
	}
}

/* padding for type string elements */
void padding_string(char *name) {
	int nr = 30, i;
	int size = strlen(name);
	//print cell name
	printf("%s", name);
	//print 30-strlen(name) spaces
	for (i = size; i <= nr; i++)
		printf(" ");
}

/* padding for type int elements */
void padding_int(int val) {
	int nr = 30, i, size=0;
	int copy = val;
	if (val == 0) size = 1;
	else {
		//how many characters the number has
		while (copy) copy /= 10, size++;
	}
	printf("%d", val);
	//print spaces
	for (i = size; i <= nr; i++)
		printf(" ");
}

/* padding for type float elements */
void padding_float(float val) {
	int nr = 30, i, size=0;
	//cast to int to find out how many characters before "."
	float copy = (int)val;
	if (copy == 0) size = 1;
	else {
		while (copy) copy /= 10, size++;
	}
	printf("%.6f", val);
	//add "." and 6 more decimals
	size += 7;
	for (i = size; i <= nr; i++)
		printf(" ");
}

/* delimiter between columns and lines */
void separator() {
	int i;
	for (i = 1; i <= 30; i++)
		printf("-");
	printf(" ");
}

/* print table "name" from database */
void PRINT_TABLE(t_db *db, char *name) {
	int nr_col = 0;
	t_table *p = searchTable(db, name);

	if (p) {
		printf("TABLE: %s\n",p->name);
		//check if there are columns
		if (!p->columns)
            return;
		t_column *c = p->columns;
		for (; c != NULL; c = c->next) {
			padding_string(c->name);
			nr_col++;
		}
		printf("\n");
		while (nr_col) {
			separator();
			nr_col--;
		}
		//check if there are lines
		printf("\n");
		if (!p->lines) {
			printf("\n");
			return;
		}
		//find datatype of elements and then print
		if (p->type == 2) {			//STRING
			t_stringLine *line = p->lines;
			for (; line != NULL; line = line->next) {
				t_stringCell *cell = line->cells;
				if (!line->cells) continue;
				for (; cell != NULL; cell = cell->next)
					padding_string(cell->value);
				printf("\n");
			}
		} else if (p->type == 0) {			//INT
			t_intLine *line = p->lines;
			for (; line != NULL; line = line->next) {
				t_intCell *cell = line->cells;
				if (!line->cells) continue;
				for (; cell != NULL; cell = cell->next)
					padding_int(cell->value);
				printf("\n");
			}
		} else if (p->type == 1) {			//FLOAT
			t_floatLine *line = p->lines;
			for (; line != NULL; line = line->next) {
				t_floatCell *cell = line->cells;
				if (!line->cells) continue;
				for (; cell != NULL; cell = cell->next)
					padding_float(cell->value);
				printf("\n");
			}
		}
		printf("\n");
	}
}

/* delete all lines from table "name" */
void CLEAR(t_db *db, char *name) {
	t_table *p = searchTable(db, name);
	if (!p) return;

	//delete lines based on datatype
	if (p->type == 2) {			//STRING
		if (p->lines == NULL) free(p->lines);
		else {
			t_stringLine *line, *nextLine;
			t_stringCell *cell, *nextCell;
			for (line = p->lines; line != NULL; line = line->next) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell->value);
					free(cell);
					cell = nextCell;
				}
			}
			line = p->lines;
			while (line) {
				nextLine = line->next;
				free(line);
				line = nextLine;
			}
		}
	} else if (p->type == 0) {			//INT
		if (p->lines == NULL) free(p->lines);
		else {
			t_intLine *line, *nextLine;
			t_intCell *cell, *nextCell;
			for (line = p->lines; line != NULL; line = line->next) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell);
					cell = nextCell;
				}
			}
			line = p->lines;
			while (line) {
				nextLine = line->next;
				free(line);
				line = nextLine;
			}
		}
	} else if (p->type == 1) {			//FLOAT
		if (p->lines == NULL) free(p->lines);
		else {
			t_floatLine *line, *nextLine;
			t_floatCell *cell, *nextCell;
			for (line = p->lines; line != NULL; line = line->next) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell);
					cell = nextCell;
				}
			}
			line = p->lines;
			while (line) {
				nextLine = line->next;
				free(line);
				line = nextLine;
			}
		}
	}
}

/* print database */
void PRINT_DB(t_db *db) {
	printf("DATABASE: %s\n\n", db->name);
	if (!db->tables) {
		return;
	}
	t_table *p = db->tables;
	//print each table
	for (; p != NULL; p = p->next) {
		PRINT_TABLE(db, p->name);
	}
}

/*
* Comparing method for type string values.
* @param rel -> the relation to be used for comparing
* @param cell -> the value this method is used for, element of table line
* @param value -> the value to be compared to
* @return - {@code 1} the relation between the 2 values is true
            {@code 0} if not
*/
int compRelString(char *rel, t_stringCell *cell, char *value) {
	if (strcmp(rel, "<") == 0) {
		if (strcmp(cell->value, value) < 0) return 1;
	} else if (strcmp(rel, "<=") == 0) {
		if (strcmp(cell->value, value) < 0 || strcmp(cell->value, value) == 0) return 1;
	} else if (strcmp(rel, "==") == 0) {
		if (strcmp(cell->value, value) == 0) return 1;
	} else if (strcmp(rel, "!=") == 0) {
		if (strcmp(cell->value, value)) return 1;
	} else if (strcmp(rel, ">=") == 0) {
		if (strcmp(cell->value, value) > 0 || strcmp(cell->value, value) == 0) return 1;
	} else if (strcmp(rel, ">") == 0) {
		if (strcmp(cell->value, value) > 0) return 1;
	}
	return 0;
}

/*
* Comparing method for type int values.
* @param rel -> the relation to be used for comparing
* @param cell -> the value this method is used for, element of table line
* @param value -> the value to be compared to
* @return - {@code 1} the relation between the 2 values is true
            {@code 0} if not
*/
int compRelInt(char *rel, t_intCell *cell, int val) {
	if (strcmp(rel, "<") == 0) {
		if (cell->value < val) return 1;
	} else if (strcmp(rel, "<=") == 0) {
		if (cell->value <= val) return 1;
	} else if (strcmp(rel, "==") == 0) {
		if (cell->value == val) return 1;
	} else if (strcmp(rel, "!=") == 0) {
		if (cell->value != val) return 1;
	} else if (strcmp(rel, ">=") == 0) {
		if (cell->value >= val) return 1;
	} else if (strcmp(rel, ">") == 0) {
		if (cell->value > val) return 1;
	}
	return 0;
}

/*
* Comparing method for type float values.
* @param rel -> the relation to be used for comparing
* @param cell -> the value this method is used for, element of table line
* @param value -> the value to be compared to
* @return - {@code 1} the relation between the 2 values is true
            {@code 0} if not
*/
int compRelFloat(char *rel, t_floatCell *cell, float val) {
	if (strcmp(rel, "<") == 0) {
		if (cell->value < val) return 1;
	} else if (strcmp(rel, "<=") == 0) {
		if (cell->value <= val) return 1;
	} else if (strcmp(rel, "==") == 0) {
		if (cell->value == val) return 1;
	} else if (strcmp(rel, "!=") == 0) {
		if (cell->value != val) return 1;
	} else if (strcmp(rel, ">=") == 0) {
		if (cell->value >= val) return 1;
	} else if (strcmp(rel, ">") == 0) {
		if (cell->value > val) return 1;
	}
	return 0;
}

/* search lines in table "name" for which the relation "rel" given as paramater is true */
void SEARCH(t_db *db, char *name, char *col_name, char *rel, char *value) {
	int i, nr_col = 0;
	char *quote = "\"";
	t_table *p = searchTable(db, name);
	if (!p) return;
	//found table
	if (!p->columns) {
		//no columns
		printf("Table %s%s%s does not contain column %s%s%s.\n", quote, name, quote, quote, col_name, quote);
		return;
	}
	t_column *c = p->columns;
	int nr = 0;
	//search for specific column "col_name"
	for (; c != NULL; c = c->next) {
		nr++;
		if (strcmp(col_name, c->name) == 0) break;
	}
	if (!c) {
		//no good column found
		printf("Table %s%s%s does not contain column %s%s%s.\n", quote, name, quote, quote, col_name, quote);
		return;
	}
	printf("TABLE: %s\n",p->name);
	for (c = p->columns; c != NULL; c = c->next) {
		//print columns
		nr_col++;
		padding_string(c->name);
	}
	printf("\n");
	while (nr_col) {
		//print delimiter between columns and lines
		separator();
		nr_col--;
	}
	printf("\n");
	if (!p->lines) {
		printf("\n");
		return;
	}
	//lines
	if (p->type == 2) {			//STRING
		t_stringLine *line = p->lines;
		for (; line != NULL; line = line->next) {
			t_stringCell *cell = line->cells;
            //go to specific column and use the comparing method
			for (i = 1; i < nr; i++) cell = cell->next;
			if (compRelString(rel, cell, value)) {
				//relation is true so print line
				for (cell = line->cells; cell != NULL; cell = cell->next)
					padding_string(cell->value);
				printf("\n");
			}
		}
	} else if (p->type == 0) {			//INT
		int val = atoi(value);
		t_intLine *line = p->lines;
		for (; line != NULL; line = line->next) {
			t_intCell *cell = line->cells;
			for (i = 1; i < nr; i++) cell = cell->next;
			if (compRelInt(rel, cell, val)) {
				for (cell = line->cells; cell != NULL; cell = cell->next)
					padding_int(cell->value);
				printf("\n");
			}
		}
	} else if (p->type == 1) {			//FLOAT
		t_floatLine *line = p->lines;
		float val = strtod(value, NULL);
		for (; line != NULL; line = line->next) {
			t_floatCell *cell = line->cells;
			for (i = 1; i < nr; i++) cell = cell->next;
			if (compRelFloat(rel, cell, val)) {
				for (cell = line->cells; cell != NULL; cell = cell->next)
					padding_float(cell->value);
				printf("\n");
			}
		}
	}
	printf("\n");
}

/* delete lines in table "name" for which the relation "rel" given as paramater is true */
void DELETE_LINE(t_db *db, char *name, char *col_name, char *rel, char *value) {
	int i;
	char *quote = "\"";
	t_table *p = searchTable(db, name);
	if (!p) return;
	//found table
	if (!p->columns) {
		printf("Table %s%s%s does not contain column %s%s%s.\n", quote, name, quote, quote, col_name, quote);
		return;
	}
	t_column *c = p->columns;
	int nr = 0;
	//search for specific column "col_name"
	for (; c != NULL; c = c->next) {
		nr++;
		if (strcmp(col_name, c->name) == 0) break;
	}
	if (!c) {
		printf("Table %s%s%s does not contain column %s%s%s.\n", quote, name, quote, quote, col_name, quote);
		return;
	}
	if (!p->lines) return;

	if (p->type == 2) {			//STRING
		t_stringLine *line = p->lines;
		t_stringLine *ant_line = NULL;
		while (line != NULL) {
			t_stringCell *cell = line->cells;
			t_stringCell *nextCell;
			//go to specific column and use the comparing method
			for (i = 1; i < nr; i++) cell = cell->next;
			if (compRelString(rel, cell, value)) {
				//relation is true so remove line from table
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell->value);
					free(cell);
					cell = nextCell;
				}
				line->cells = NULL;
				if (ant_line == NULL) p->lines = line->next; //first line
				else if (line->next == NULL) ant_line->next = NULL;	//last line
				else ant_line->next = line->next;
				free(line);
				if (!ant_line && !p->lines) return;	//only line in table
				else if (!ant_line) line = p->lines; //first but not the only one
				else line = ant_line->next; //anything else
			} else {
				ant_line = line;
				line = line->next;
			}
		}
	} else if (p->type == 0) {		//INT
		int val = atoi(value);
		t_intLine *line = p->lines;
		t_intLine *ant_line = NULL;
		while (line != NULL) {
			t_intCell *cell = line->cells;
			t_intCell *nextCell;
			for (i = 1; i < nr; i++) cell = cell->next;
			if (compRelInt(rel, cell, val)) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell);
					cell = nextCell;
				}
				line->cells = NULL;
				if (ant_line == NULL) p->lines = line->next;
				else if (line->next == NULL) ant_line->next = NULL;
				else ant_line->next = line->next;
				free(line);
				if (!ant_line && !p->lines) return;
				else if (!ant_line) line = p->lines;
				else line = ant_line->next;
			} else {
				ant_line = line;
				line = line->next;
			}
		}
	} else if (p->type == 1) {		//FLOAT
		t_floatLine *line = p->lines;
		t_floatLine *ant_line = NULL;
		float val = strtod(value, NULL);
		while (line != NULL)  {
			t_floatCell *cell = line->cells;
			t_floatCell *nextCell;
			for (i = 1; i < nr; i++) cell = cell->next;
			if (compRelFloat(rel, cell, val)) {
				cell = line->cells;
				while (cell) {
					nextCell = cell->next;
					free(cell);
					cell = nextCell;
				}
				line->cells = NULL;
				if (ant_line == NULL) p->lines = line->next;
				else if (line->next == NULL) ant_line->next = NULL;
				else ant_line->next = line->next;
				free(line);
				if (!ant_line && !p->lines) return;
				else if (!ant_line) line = p->lines;
				else line = ant_line->next;
			} else {
				ant_line = line;
				line = line->next;
			}

		}
	}
}

/* delete database */
void DELETE_DB(t_db *db) {
	if (!db->tables) {
		free(db->tables);
		free(db);
		return;
	}
	t_table *p;
	
	//delete each table
	while (db->tables) {
		p = db->tables;
		while (p->next) p = p->next;
		DELETE_TABLE(db, p->name);
	}
	free(db);
}

/* free memory of given pointer array */
void free_array(char **array) {
	int i;
	for (i = 0; i < MAX_VAL; i++)
        free(array[i]);
	free(array);
}

/*
* Main function in which the input is verified and then separated
* by using "strtok". The values obtained are then sent to the
* implemented functions as parameters.
*/
int main() {
	t_db* db = NULL;
	char *input = malloc(MAX_CMD_LEN);

	while (fgets(input,MAX_CMD_LEN,stdin)) {
		if (strcmp(input,"DELETE_DB\n") == 0) {
			DELETE_DB(db);
			break;
		} else if (strstr(input,"INIT_DB"))	{
			char *token = strtok(input, " ");
			token = strtok(NULL, " ");
			token[strlen(token) - 1] = 0;
			db = INIT_DB(token);
		} else if (strstr(input,"CREATE")) {
			int nr = 0, i;
			char *name = malloc(MAX_TABLE_NAME_LEN);
			char *type = malloc(MAX_VAL);
			char **columns = malloc(MAX_VAL*MAX_COLUMN_NAME_LEN);
			for (i = 0; i < MAX_VAL; i++)
        		columns[i] = malloc(MAX_COLUMN_NAME_LEN);

			char *token = strtok(input, " ");
			token = strtok(NULL, " ");
			strcpy(name, token);
			token = strtok(NULL, " ");
			strcpy(type, token);
			token = strtok(NULL, " ");

			while (token) {
				nr++;
				strcpy(columns[nr-1],token);
				token = strtok(NULL, " ");
			}
			columns[nr-1][strlen(columns[nr-1]) - 1] = 0;
			insTable(db, name, type, columns, &nr);
			free(name);
			free(type);
			free_array(columns);
		} else if (strstr(input,"DELETE ")) {
			char *name = malloc(MAX_TABLE_NAME_LEN);
			char *token = strtok(input, " ");
			token = strtok(NULL, " ");
			strcpy(name,token);
			if (strstr(name,"\n")) name[strlen(name)-1] = 0;
			token = strtok(NULL, " ");
			if (!token) DELETE_TABLE(db, name);
			else {
				char *col_name = malloc(MAX_COLUMN_NAME_LEN);
				char *rel = malloc(MAX_REL);
				char *value = malloc(MAX_VAL);

				strcpy(col_name, token);
				token = strtok(NULL, " ");
				strcpy(rel, token);
				token = strtok(NULL, " ");
				strcpy(value, token);

				value[strlen(value) - 1] = 0;
				DELETE_LINE(db, name, col_name, rel, value);
				free(col_name);
				free(rel);
				free(value);
			}
			free(name);
		} else if (strstr(input,"ADD ")) {
			int nr_col = 0, i;
			char *name = malloc(MAX_TABLE_NAME_LEN);
			char **values = malloc(MAX_VAL*MAX_COLUMN_NAME_LEN);
			for (i = 0; i < MAX_VAL; i++)
        		values[i] = malloc(MAX_COLUMN_NAME_LEN);
			char *token = strtok(input, " ");
			token = strtok(NULL, " ");
			strcpy(name,token);
			token = strtok(NULL, " ");
			while (token) {
				nr_col++;
				strcpy(values[nr_col-1],token);
				token = strtok(NULL, " ");
			}
			values[nr_col-1][strlen(values[nr_col-1]) - 1] = 0;
			ADD(db, name, values, &nr_col);
			free(name);
			free_array(values);
		} else if (strcmp(input,"PRINT_DB\n") == 0) {
			PRINT_DB(db);
		} else if (strstr(input,"PRINT ")) {
			char *token = strtok(input, " ");
			token = strtok(NULL, " ");
			char *name = malloc(MAX_TABLE_NAME_LEN);
			strcpy(name, token);
			name[strlen(name) - 1] = 0;
			PRINT_TABLE(db, name);
			free(name);
		} else if (strstr(input,"CLEAR ")) {
			char *token = strtok(input, " ");
			token = strtok(NULL, " ");
			char *name = malloc(MAX_TABLE_NAME_LEN);
			strcpy(name, token);
			name[strlen(name) - 1] = 0;
			CLEAR(db, name);
			free(name);
		} else if (strstr(input,"SEARCH")) {
			char *name = malloc(MAX_TABLE_NAME_LEN);
			char *col_name = malloc(MAX_COL);
			char *rel = malloc(MAX_REL);
			char *value = malloc(MAX_VAL);
			char *token = strtok(input, " ");

			token = strtok(NULL, " ");
			strcpy(name, token);
			token = strtok(NULL, " ");
			strcpy(col_name, token);
			token = strtok(NULL, " ");
			strcpy(rel, token);
			token = strtok(NULL, " ");
			strcpy(value, token);

			value[strlen(value) - 1] = 0;
			SEARCH(db, name, col_name, rel, value);
			free(name);
			free(col_name);
			free(rel);
			free(value);
		} else printf("Unknown command: %s", input);
	}
	free(input);
	return 0;
}
