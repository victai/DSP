#include "hmm.h"
#include <stdlib.h>
#include <stdio.h>

int main( int argc, char* argv[] ) {
	HMM hmm_models[5];
	int model_cnt = load_models(argv[1], hmm_models, 5);
	int best_model = -1;
	FILE *fin = open_or_die(argv[2], "r");
	FILE *result_file = open_or_die(argv[3], "w");
	//FILE *ans_file = open_or_die("../testing_answer.txt", "r");
	FILE *acc_file = open_or_die("acc.txt", "a");
	char s[MAX_SEQ], ans[MAX_SEQ];
	double acc = 0;
	int cnt = 0;
	while ( fscanf(fin, "%s", s) != EOF ) {
		int seq_len = strlen(s);
		double best_prob = -1;
		cnt++;
		for ( int m = 0; m < model_cnt; m++ ) {
			double probability[MAX_SEQ][MAX_STATE] = {{0}};
			for ( int t = 0; t < seq_len; t++ ) {
				for ( int i = 0; i < hmm_models[m].state_num; i++ ) {
					double max = -1;
					if ( t == 0 ) {
						probability[t][i] = hmm_models[m].initial[i];
						max = probability[t][i];
					}
					else {
						for ( int j = 0; j < hmm_models[m].state_num; j++ ) {
							if ( probability[t-1][j] * hmm_models[m].transition[j][i] > max )
								max = probability[t-1][j] * hmm_models[m].transition[j][i];
						}
					}
					probability[t][i] = max * hmm_models[m].observation[s[t]-'A'][i];
				}
			}
			double final_max = -1;
			for ( int i = 0; i < hmm_models[m].state_num; i++ )
				final_max = (probability[seq_len-1][i] > final_max)? probability[seq_len-1][i] : final_max;
			if ( final_max > best_prob ) {
				best_prob = final_max;
				best_model = m;
			}
		}
		fprintf(result_file, "%s %e\n", hmm_models[best_model].model_name, best_prob);
		//fscanf(ans_file, "%s", ans);
		//if ( strcmp(hmm_models[best_model].model_name, ans) == 0 )	acc++;
	}
	//acc /= cnt;
	//printf("acc: %f\n", acc);
	//fprintf(acc_file, "%f\n", acc);
	fclose(fin);
	fclose(result_file);
	//fclose(ans_file);
	//fclose(acc_file);

	return 0;
}
