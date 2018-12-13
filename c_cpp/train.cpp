#include "hmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main ( int argc, char* argv[] ) {
	int iteration = atoi(argv[1]);
	HMM hmm;
	loadHMM( &hmm, argv[2] );

	while ( iteration-- ){
		FILE *fin = open_or_die( argv[3], "r" );
		char s[256];
		int cnt = 0;
		double cumu_init[MAX_STATE] = {0};
		double cumu_gamma[MAX_STATE] = {0};
		double cumu_epsilon[MAX_STATE][MAX_STATE] = {{0}};
		double VK[MAX_STATE][MAX_OBSERV] = {{0}};
		while ( fscanf( fin, "%s", s ) != EOF ) {
			cnt++;
			int seq_len = strlen(s);
			double Alpha[MAX_SEQ][MAX_STATE] = {{0}};
			for( int t = 0; t < seq_len; t++ ) {
				for ( int i = 0; i < hmm.state_num; i++ ) {
					if ( t == 0 )
						Alpha[t][i] = hmm.initial[i] * hmm.observation[s[t]-'A'][i];
					else {
						for ( int j = 0; j < hmm.state_num; j++ )
							Alpha[t][i] += Alpha[t-1][j] * hmm.transition[j][i];
						Alpha[t][i] *= hmm.observation[s[t]-'A'][i];
					}
				}	
			}

			double Beta[MAX_SEQ][MAX_STATE] = {{0}};
			for ( int t = seq_len-1; t >= 0; t-- ) {
				for ( int i = 0; i < hmm.state_num; i++ ) {
					if ( t == seq_len-1 )
						Beta[t][i] = 1;
					else {
						for ( int j = 0; j < hmm.state_num; j++ )
							Beta[t][i] += hmm.transition[i][j] * hmm.observation[s[t+1]-'A'][j] * Beta[t+1][j];
					}
				}
			}
			/*
			printf("-----------");
			for(int t = 0; t < seq_len; t++){
				for(int i =0 ;i < hmm.state_num; i++)
					printf("%lf ", Beta[t][i]);
				printf("\n");
			}
			if (cnt == 100) exit(0);*/

			double Gamma[MAX_SEQ][MAX_STATE] = {{0}};
			for ( int t = 0; t < seq_len; t++ ) {
				double gamma_sum = 0;
				for ( int i = 0; i < hmm.state_num; i++ )
					gamma_sum += Alpha[t][i] * Beta[t][i];
				for ( int i = 0; i < hmm.state_num; i++ )
					Gamma[t][i] = Alpha[t][i] * Beta[t][i] / gamma_sum;
			}

			double Epsilon[MAX_SEQ-1][MAX_STATE][MAX_STATE] = {{{0}}};
			for ( int t = 0; t < seq_len-1; t++ ) {
				double epsilon_sum = 0;
				for ( int i = 0; i < hmm.state_num; i++ ) {
					for ( int j = 0; j < hmm.state_num; j++ ) {
						Epsilon[t][i][j] += Alpha[t][i] * hmm.transition[i][j] * hmm.observation[s[t+1]-'A'][j] * Beta[t+1][j];
						epsilon_sum += Epsilon[t][i][j];
					}
				}
				for ( int i = 0; i < hmm.state_num; i++ )
					for ( int j = 0; j < hmm.state_num; j++ )
						Epsilon[t][i][j] /= epsilon_sum;
			}

			for ( int t = 0; t < seq_len-1; t++ ) {
				for ( int i = 0; i < hmm.state_num; i++ ){
					if ( t == 0 )	cumu_init[i] += Gamma[t][i];
					cumu_gamma[i] += Gamma[t][i];
					VK[i][s[t]-'A'] += Gamma[t][i];
					for ( int j = 0; j < hmm.state_num; j++ )
						cumu_epsilon[i][j] += Epsilon[t][i][j];
				}
			}
		}


		for ( int i = 0; i < hmm.state_num; i++ ) {		// pi
			hmm.initial[i] = cumu_init[i] / cnt;
		}
		for ( int i = 0; i < hmm.state_num; i++ ) {		// a
			for ( int j = 0; j < hmm.state_num; j++ ) {
				hmm.transition[i][j] = cumu_epsilon[i][j] / cumu_gamma[i];
			}
		}
		for ( int i = 0; i < hmm.state_num; i++ ) { 	// b
			for ( int o = 0; o < MAX_OBSERV; o++) {
				hmm.observation[o][i] = VK[i][o] / cumu_gamma[i];
			}
		}
		fclose(fin);
	}

	FILE *fout = open_or_die( argv[4], "w" );
	dumpHMM(fout, &hmm);


	return 0;
}
