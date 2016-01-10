/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2006, Xorcom
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "hexfile.h"

static const char rcsid[] = "$Id: hexfile.c 2477 2007-04-29 22:12:49Z tzafrir $";

static parse_hexfile_report_func_t	report_func = NULL;

parse_hexfile_report_func_t parse_hexfile_set_reporting(parse_hexfile_report_func_t rf)
{
	parse_hexfile_report_func_t	old_rf = report_func;
	report_func = rf;
	return old_rf;
}

static void chomp(char buf[])
{
	size_t	last = strlen(buf) - 1;
	while(last >= 0 && (buf[last] == '\n' || buf[last] == '\r'))
		buf[last--] = '\0';
}

int checksum(struct hexline *hexline)
{
	unsigned int	i;
	unsigned int	chksm = 0;
	int		ll = hexline->d.content.header.ll;

	for(i = 0; i <= sizeof(hexline->d.content.header) + ll; i++) {
		chksm += hexline->d.raw[i];
	}
	return chksm & 0xFF;
}

int dump_hexline(int recordno, struct hexline *line, FILE *outfile)
{
	uint8_t		ll;
	uint16_t	offset;
	uint8_t		tt;
	uint8_t		old_chksum;
	uint8_t		new_chksum;
	uint8_t		*data;
	unsigned int	i;

	ll = line->d.content.header.ll;
	offset = line->d.content.header.offset;
	tt = line->d.content.header.tt;
	fprintf(outfile, ":%02X%04X%02X", ll, offset, tt);
	data = line->d.content.tt_data.data;
	for(i = 0; i < ll; i++) {
		fprintf(outfile, "%02X", data[i]);
	}
	old_chksum = data[ll];
	data[ll] = 0;
	new_chksum = 0xFF - checksum(line) + 1;
	data[ll] = old_chksum;
	fprintf(outfile, "%02X\n", new_chksum);
	if(new_chksum != old_chksum) {
		if(report_func)
			report_func(LOG_ERR, "record #%d: new_chksum(%02X) != old_chksum(%02X)\n",
					recordno, new_chksum, old_chksum);
		return 0;
	}
	return 1;
}

static int update_hexline(struct hexdata *hexdata, char *buf)
{
	int		ret;
	unsigned int	ll, offset, tt;
	char		*p;
	struct hexline	*hexline;
	unsigned int	i;
	int		allocsize;
	unsigned int	last_line = hexdata->last_line;

	if(hexdata->got_eof) {
		if(report_func)
			report_func(LOG_ERR, "Extranous data after EOF record\n");
		return -EINVAL;
	}
	if(last_line >= hexdata->maxlines) {
		if(report_func)
			report_func(LOG_ERR, "Hexfile too large (maxline %d)\n", hexdata->maxlines);
		return -ENOMEM;
	}
	ret = sscanf(buf, "%02X%04X%02X", &ll, &offset, &tt);
	if(ret != 3) {
		if(report_func)
			report_func(LOG_ERR, "Bad line header (only %d items out of 3 parsed)\n", ret);
		return -EINVAL;
	}
	switch(tt) {
		case TT_DATA:
			break;
		case TT_EOF:
			if(ll != 0) {
				if(report_func)
					report_func(LOG_ERR, "Bad EOF record len = %d\n", ll);
				return -EINVAL;
			}
			if(offset != 0) {
				if(report_func)
					report_func(LOG_ERR, "Bad EOF record offset = %d\n", offset);
				return -EINVAL;
			}
			hexdata->got_eof = 1;
			break;
		case TT_EXT_SEG:	/* Unimplemented */
		case TT_START_SEG:	/* Unimplemented */
		case TT_EXT_LIN:	/* Unimplemented */
		case TT_START_LIN:	/* Unimplemented */
			return 1;
		default:
			if(report_func)
				report_func(LOG_ERR, "Unknown record type %d\n", tt);
			return -EINVAL;
	}
	buf += 8;	/* Skip header */
	ll++;		/* include the checksum for now */
	allocsize = sizeof(struct hexline) + ll;
	if((hexline = (struct hexline *)malloc(allocsize)) == NULL) {
		if(report_func)
			report_func(LOG_ERR, "No more memory for hexfile lines\n");
		return -EINVAL;
	}
	memset(hexline, 0, allocsize);
	hexline->d.content.header.ll = ll;
	hexline->d.content.header.offset = offset;
	hexline->d.content.header.tt = tt;
	hexdata->lines[last_line++] = hexline;
	p = buf;
	for(i = 0; i < ll; i++) {
		unsigned int	val;

		if((*p == '\0') || (*(p+1) == '\0')) {
			if(report_func)
				report_func(LOG_ERR, "Short data string '%s'\n", buf);
			return -EINVAL;
		}
		ret = sscanf(p, "%02X", &val);
		if(ret != 1) {
			if(report_func)
				report_func(LOG_ERR, "Bad data byte #%d\n", i);
			return -EINVAL;
		}
		hexline->d.content.tt_data.data[i] = val;
		p += 2;
	}
	hexline->d.content.header.ll--;	/* Fix the checksum */
	if(checksum(hexline) != 0) {
		if(report_func) {
			report_func(LOG_ERR, "Bad checksum (%d instead of 0)\n", checksum(hexline));
			dump_hexline(last_line, hexline, stderr);
		}
		return -EINVAL;
	}
	if(hexdata->got_eof)
		return 0;
	hexdata->last_line++;
	return 1;
}

void free_hexdata(struct hexdata *hexdata)
{
	if(hexdata) {
		unsigned int	i;

		for(i = 0; i < hexdata->maxlines; i++)
			if(hexdata->lines[i] != NULL)
				free(hexdata->lines[i]);
		free(hexdata);
	}
}

int dump_hexfile(struct hexdata *hexdata, FILE *outfile)
{
	unsigned int	i;

	for(i = 0; i <= hexdata->last_line; i++) {
		struct hexline	*line = hexdata->lines[i];
		if(!line) {
			if(report_func)
				report_func(LOG_ERR, "Missing line at #%d\n", i);
			return -EINVAL;
		}
		if(!dump_hexline(i, line, outfile))
			return -EINVAL;
	}
	return 0;
}

int dump_hexfile2(struct hexdata *hexdata, FILE *outfile, uint8_t maxwidth)
{
	uint8_t		ll;
	uint8_t		tt;
	uint8_t		new_chksum;
	uint8_t		*data;
	unsigned int	i;
	unsigned int	j;

	if (maxwidth <= sizeof(hexdata->lines[0]->d.content.header) ){
		if(report_func) 
			report_func(LOG_ERR, "Line width too small %d\n", maxwidth);
		return -EINVAL;
	}
	
	for(i = 0; i <= hexdata->last_line; i++) {
		struct hexline	*line = hexdata->lines[i];
		struct hexline *extraline;
		int allocsize;
		int bytesleft = 0;
		int extra_offset = 0;
		unsigned int this_line = 0;
		
		if(!line) {
			if(report_func)
				report_func(LOG_ERR, "Missing line at #%d\n", i);
			return -EINVAL;
		}
		ll = line->d.content.header.ll;
		bytesleft = ll;
		/* split the line into several lines */
		tt = line->d.content.header.tt;
		while (bytesleft > 0) {
			this_line = (bytesleft >= maxwidth) ? maxwidth : bytesleft;
			allocsize = sizeof(struct hexline) + this_line + 1;
			/* generate the new line */
			if((extraline = (struct hexline *)malloc(allocsize)) == NULL) {
				if(report_func)
					report_func(LOG_ERR, "No more memory for hexfile lines\n");
				return -EINVAL;
			}
			memset( extraline, 0, allocsize );
			extraline->d.content.header.ll		= this_line;
			extraline->d.content.header.offset	= line->d.content.header.offset + extra_offset;
			extraline->d.content.header.tt		= tt;
			memcpy( extraline->d.content.tt_data.data, line->d.content.tt_data.data+extra_offset, this_line);
			new_chksum = 0xFF - checksum(extraline) + 1;
			/* print it */
			data = extraline->d.content.tt_data.data;
			fprintf(outfile, ":%02X%04X%02X", extraline->d.content.header.ll, extraline->d.content.header.offset, tt);
			for(j = 0; j < this_line; j++) {
				fprintf(outfile, "%02X", data[j]);
			}
			fprintf(outfile, "%02X\n", new_chksum);
			/* cleanups */
			free( extraline);
			extra_offset += this_line;
			bytesleft -= this_line;
		}
	}
	return 0;
}

void process_comment(struct hexdata *hexdata, char buf[])
{
	char		*dollar_start;
	char		*dollar_end;
	const char	id_prefix[] = "Id: ";
	char		tmp[BUFSIZ];
	char		*p;
	int		len;

	if(report_func)
		report_func(LOG_INFO, "Comment: %s\n", buf + 1);
	/* Search for RCS keywords */
	if((dollar_start = strchr(buf, '$')) == NULL)
		return;
	if((dollar_end = strchr(dollar_start + 1, '$')) == NULL)
		return;
	/* Crop the '$' signs */
	len = dollar_end - dollar_start;
	len -= 2;
	memcpy(tmp, dollar_start + 1, len);
	tmp[len] = '\0';
	p = tmp;
	if(strstr(tmp, id_prefix) == NULL)
		return;
	p += strlen(id_prefix);
	if((p = strchr(p, ' ')) == NULL)
		return;
	p++;
	strncpy(hexdata->version_info, p, BUFSIZ - 1);
	if((p = strchr(hexdata->version_info, ' ')) != NULL)
		*p = '\0';
}

struct hexdata *parse_hexfile(const char *fname, unsigned int maxlines)
{
	FILE			*fp;
	struct hexdata		*hexdata = NULL;
	int			datasize;
	char			buf[BUFSIZ];
	int			line;
	int			ret;

	assert(fname != NULL);
	if(report_func)
		report_func(LOG_INFO, "Parsing %s\n", fname);
	datasize = sizeof(struct hexdata) + maxlines * sizeof(char *);
	hexdata = (struct hexdata *)malloc(datasize);
	if(!hexdata) {
		if(report_func)
			report_func(LOG_ERR, "Failed to allocate %d bytes for hexfile contents\n", datasize);
		goto err;
	}
	memset(hexdata, 0, datasize);
	hexdata->maxlines = maxlines;
	if((fp = fopen(fname, "r")) == NULL) {
		if(report_func)
			report_func(LOG_ERR, "Failed to open hexfile '%s'\n", fname);
		goto err;
	}
	line = 0;
	while(fgets(buf, BUFSIZ, fp)) {
		line++;
		if(buf[0] == '\0') {
				if(report_func)
					report_func(LOG_ERR, "Short line at %s:%d\n", fname, line);
				goto err;
		}
		chomp(buf);
		if(buf[0] == '#') {
			process_comment(hexdata, buf);
			continue;
		}
		if(buf[0] != ':') {
			if(report_func)
				report_func(LOG_ERR, "Line begins with 0x%X at %s:%d\n", buf[0], fname, line);
			goto err;
		}
		if((ret = update_hexline(hexdata, buf + 1)) < 0) {
			if(report_func)
				report_func(LOG_ERR, "Failed parsing %s at line: %d\n", fname, line);
			goto err;
		}
	}
	fclose(fp);
	if(report_func)
		report_func(LOG_INFO, "%s parsed OK\n", fname);
	return hexdata;
err:
	free_hexdata(hexdata);
	return NULL;
}

void dump_binary(struct hexdata *hexdata, const char *outfile)
{
	FILE		*fp;
	unsigned int	i;
	size_t		len;

	if(report_func)
		report_func(LOG_INFO, "Dumping binary data into '%s'\n", outfile);
	if((fp = fopen(outfile, "w")) == NULL) {
		perror(outfile);
		exit(1);
	}
	for(i = 0; i < hexdata->maxlines; i++) {
		struct hexline	*hexline = hexdata->lines[i];

		if(!hexline)
			break;
		if(hexline->d.content.header.tt == TT_EOF) {
			if(report_func)
				report_func(LOG_INFO, "\ndump: good EOF record");
			continue;
		}
		if(report_func)
			report_func(LOG_INFO, "dump: %6d\r", i);
		len = hexline->d.content.header.ll;
		if(fwrite(hexline->d.content.tt_data.data, 1, len, fp) != len) {
			perror("write");
			exit(1);
		}
	}
	if(report_func)
		report_func(LOG_INFO, "\nDump finished\n");
	fclose(fp);
}

/*
 * Algorithm lifted of sum(1) implementation from coreutils.
 * We chose the default algorithm (BSD style).
 */
int calc_checksum(struct hexdata *hexdata)
{
	unsigned int	i;
	size_t		len;
	int		ck = 0;

	for(i = 0; i < hexdata->maxlines; i++) {
		struct hexline	*hexline = hexdata->lines[i];
		unsigned char	*p;

		if(!hexline)
			break;
		if(hexline->d.content.header.tt == TT_EOF)
			continue;
		len = hexline->d.content.header.ll;
		p = hexline->d.content.tt_data.data;
		for(; len; p++, len--) {
			ck = (ck >> 1) + ((ck & 1) << 15);
			ck += *p;
			ck &= 0xffff;	/* Keep it within bounds. */
		}
	}
	return ck;
}
