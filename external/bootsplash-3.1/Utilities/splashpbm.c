#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
  int w, h, x, y, c, xx, yy, yl, xs, ys, px, py;
  int offx = 0, offy = 0;
  int anchorx = -1, anchory = -1;
  int invert = 0;
  char *p;
  FILE *f;
  unsigned char *pic;
  unsigned int d[32];
  int copy = 1;

  while(argc > 1)
    {
      if (!strcmp(argv[1], "invert"))
	{
	  invert = 1;
	  argc--;
	  argv++;
	  continue;
	}
      if (!strncmp(argv[1], "offsetx=", 8))
	{
	  offx = atoi(argv[1] + 8);
	  if (offx < 0)
	    {
	      offx = 0;
	    }
	  argc--;
	  argv++;
	  continue;
	}
      if (!strncmp(argv[1], "offsety=", 8))
	{
	  offy = atoi(argv[1] + 8);
	  if (offy < 0)
	    {
	      offy = 0;
	    }
	  argc--;
	  argv++;
	  continue;
	}
      if (!strncmp(argv[1], "copy=", 5))
	{
	  copy = atoi(argv[1] + 5);
	  if (copy < 1)
	    {
	      fprintf(stderr, "copy: illegal value '%d'\n", copy);
	      exit(1);
	    }
	  argc--;
	  argv++;
	  continue;
	}
      if (!strncmp(argv[1], "anchor=", 7))
	{
	  for (p = argv[1] + 7; (c = *p) != 0; p++)
	    {
	      switch(c)
		{
		case 't':
		  anchory = -1;
		  break;
		case 'b':
		  anchory = 1;
		  break;
		case 'l':
		  anchorx = -1;
		  break;
		case 'r':
		  anchorx = 1;
		  break;
		default:
		  fprintf(stderr, "anchor: unknown position '%c'\n", c);
		  exit(1);
		}
	    }
	  argc--;
	  argv++;
	  continue;
	}
      break;
    }
  if (argc == 1 || !strcmp(argv[1], "-"))
    f = stdin;
  else if ((f = fopen(argv[1], "r")) == 0)
    {
      perror(argv[1]);
      exit(1);
    }
  if (getc(f) != 'P' || getc(f) != '4' || getc(f) != '\n')
    {
      fprintf(stderr, "%s: not a raw pbm file.\n", argv[1] ? argv[1] : "<stdin>");
      exit(1);
    }
  while ((c = getc(f)) == '#')
    fscanf(f, "%*[^\n]\n");
  ungetc(c, f);
  fscanf(f, "%d%d%*c", &w, &h);
  pic = (unsigned char *)malloc(w * h);
  for (y = 0; y < h; y++)
    for (x = 0; x < w; )
      {
	c = getc(f);
	for (xx = 0x80; xx && x < w; xx >>= 1, x++)
	  pic[y * w + x] = c & xx ? 1 - invert : invert;
      }
  if (f != stdin)
    fclose(f);
  if (anchorx < 0)
    xs = 0;
  else
    xs = (w & 31) ? (w & 31) - 32 : 0;
  if (anchory < 0)
    ys = 0;
  else
    ys = (h & 31) ? (h & 31) - 32 : 0;

  for (y = ys; y < h; y += 32)
    {
      yl = h - y > 32 ? 32 : h - y;
      if ((anchory != -1 || 1) && yl < 5)
	yl = 5;
      for (x = xs; x < w; x += 32)
	{
	  for (yy = 0; yy < 32; yy++)
	    d[yy] = 0;
	  for (yy = 0; yy < yl; yy++)
	    for (xx = 0; xx < 32; xx++)
	      {
		px = x + xx;
		py = y + yy;
	        if (px >= 0 && px < w && py >= 0 && py < h && pic[py * w + px])
		  d[yy] |= 0x80000000 >> xx;
	      }
	  if ((yl & 3) > 1)
	    yl = yl + 3 & ~3;
	  if (x != xs || y != ys)
	    {
	      for (yy = 0; yy < copy; yy++)
	        printf("box silent copy %d\n", yy);
	    }
	  for (yy = 0; yy < yl; yy++)
	    {
	      if ((yy & 3) == 0)
		{
		  if (yy)
		    putchar('\n');
		  printf("box silent stipple");
		  if (yy == 0 && anchorx == -1)
		    printf(" range %d %d", x + offx, (x + 32 > w ? w - 1 : x + 32 - 1) + offx);
		  if (yy == 0 && anchorx == 1)
		    printf(" anchorend range %d %d", (x < 0 ? w - 1 : w - 1 - x) + offx, w - x - 32 + offx);
		  if (yy == 4 && anchory == -1)
		    printf(" range %d %d", y + offy, (y + yl > h ? h - 1 : y + yl - 1) + offy);
		  if (yy == 4 && anchory == 1)
		    printf(" anchorend range %d %d", (y < 0 ? h - 1 : h - 1 - y) + offy, h - y - yl + offy);
		}
	      printf(" #%08x", d[yy]);
	    }
	  putchar('\n');
	}
    }
  exit(0);
}
