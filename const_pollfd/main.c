#include <stdio.h>
#include <ctype.h>


int get_num_of_tracked_files(const char *path)
{
	FILE *conf = fopen(path, "r");
	int icurr;
	int strings;
	int pos, not_graph;
	while(icurr != EOF){
		/*current string*/
		pos = 0;
		not_graph = 0;
		/*получаем новую строку в str*/
		while((icurr=fgetc(conf)) != 10 && icurr != EOF){
			//*str++ = (char)icurr;
			//if(++pos >= LINE_MAX-1){
			//	break;
			//}
			pos++;
			if(!isgraph(icurr)) not_graph++;
		}
		if(pos > not_graph) strings++;
	}
	if(strings % 3){
		fprintf(stderr, "Bad config file!\n");
		return -1;
	}
	FILE *npoll = fopen("../npollfd.data", "w");
	fprintf(npoll, "%d\n", strings/3);
	return 0;
}



int main(int argc, char *argv[])
{
	if(!get_num_of_tracked_files(argv[1])) return 0;
	return -1;
}
