#include <stdio.h>

#include "wctdm_fxomodes.h"

int main() {
	size_t i;
	
	printf("case \"$mode\" in\n");
	for (i=0; i<(sizeof(fxo_modes)/sizeof(struct fxo_mode)); i++) {
		if (fxo_modes[i].name == NULL) break;
		int reg16=0, reg26=0, reg30=0, reg31=0xa3;
		char ring_osc[BUFSIZ]="", ring_x[BUFSIZ] = "";
		
		reg16 |= (fxo_modes[i].ohs << 6);
		reg16 |= (fxo_modes[i].rz << 1);
		reg16 |= (fxo_modes[i].rt);
		
		reg26 |= (fxo_modes[i].dcv << 6);
		reg26 |= (fxo_modes[i].mini << 4);
		reg26 |= (fxo_modes[i].ilim << 1);
		
		reg30 = (fxo_modes[i].acim);
		
		reg31 |= (fxo_modes[i].ohs2 << 3);

		if (fxo_modes[i].ring_osc !=0 ) {
			snprintf(ring_osc, BUFSIZ, "; ring_osc=\"%02X %02X\"",
				(fxo_modes[i].ring_osc)>>8,
				(fxo_modes[i].ring_osc)&&0xFF
			);
		}
		if (fxo_modes[i].ring_x !=0 ) {
			snprintf(ring_x, BUFSIZ, "; ring_x=\"%02X %02X\"",
				(fxo_modes[i].ring_x)>>8,
				(fxo_modes[i].ring_x)&&0xFF
			);
		}
		
		printf("%s)\treg16=%02X; reg26=%02X; reg30=%02X; reg31=%02X%s%s;;\n",
		       fxo_modes[i].name, reg16, reg26, reg30, reg31, ring_osc, ring_x);
	}
	printf("esac\n");
	return 0;
}
