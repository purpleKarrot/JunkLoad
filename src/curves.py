
nb_count = 8;
curves	 = []
curve_set	= []
old_curve_s	= []

def setup_curves():
	for i in range( nb_count ):
		for j in range( nb_count - i - 1 ):
				s = i, j+i+1
				curves.append( s )
				print( s )
	print len(curves)
	
	
	
def setup_curve_sets():
	for c in curves: 
		for c2 in curves:
			c1a, c1b = c
			c2a, c2b = c2
			if c1a < c2a and c1b < c2b and c1a != c2b and c1b != c2a:
				cs = c, c2
				curve_set.append( cs )
				print cs
	print( len( curve_set ) )



def main():
	setup_curves()
	setup_curve_sets()


if __name__ == "__main__":
    main()
