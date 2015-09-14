#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "random_numbers.h"

//useful functions
using namespace std; void FileTest(ifstream& file, string filename)
{
	cout << filename;
	if (file.is_open())
		cout << " open\n";
	else
	{
		while (!file.is_open())
		{
			cout << " not open. Please re-enter filename: ";
			getline(cin, filename, '\n');
			file.open(filename);
		}
	}
}

istream& universal_getline(istream& is, string& t)
{
	//this code is adapted from a post on stackoverflow:
	// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
	//written by user763305
	t.clear();
	istream::sentry se(is, true);
	streambuf* sb = is.rdbuf();//sets pointer to stream buffer object

	for (;;)
	{
		int c = sb->sbumpc();//get current character and advance to the next position
		switch (c)//tests for equality against a list of variables (like multiple if statements)
		{
		case '\n'://if the next character is '\n', return the line
			return is;
		case '\r'://if the character is '\n', see if the next one is '\n' or return the line
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:// Also handle the case when the last line has no line ending
			if (t.empty())//if there's nothing there, set it to be the end of file
				is.setstate(ios::eofbit);//set it to be the end of the file and return it
			return is;
		default://if none of the above, continue on.
			t += (char)c;
		}
	}

}

//adapted from Hudson (1991)'s C code
struct node {
public:
	double time;
	struct node *desc1;
	struct node *desc2;
	struct node *ancestor;
};

void make_tree(struct node *tree, int sample_size)
{
	int in, pick;
	double t, rand, x;
	struct node **list;
	//vector<node> list;

	//initialize things
	list = (struct node **)malloc(sample_size*sizeof(struct node *));
	for (in = 0; in < sample_size; in++)
	{
		tree[in].time = 0;
		tree[in].desc1 = tree[in].desc2 = 0;
		list[in] = tree + in;
		//list.push_back(tree[in]);
	}

	//generate times of the nodes
	t = 0;
	for (in = sample_size; in > 0; in--)
	{
		t += -2 * log(1 - genrand()) / (((double)in)*(in - 1));
		tree[2 * sample_size - in].time = t;
	}

	//generate topology of the tree
	for (in = sample_size; in > 1; in--)
	{
		pick = in*genrand();
		list[pick]->ancestor = tree + 2 * sample_size - in;
		//list[pick] = tree[2 * sample_size - in];
		tree[2 * sample_size - in].desc1 = list[pick];
		list[pick] = list[in - 1];
		pick = (in - 1)*genrand();
		list[pick]->ancestor = tree + 2 * sample_size - in;
		tree[2 * sample_size - in].desc2 = list[pick];
		list[pick] = tree + 2 * sample_size - in;
	}

	free(list);
}

void bottleneck(struct node *tree, int sample_size, double time, double factor)
{
	int in;
	for (in = sample_size; in < 2 * sample_size; in++)
	{
		if (tree[in].time > time)
			tree[in].time = factor*(tree[in].time - time) + time;
	}
}

int count_desc(struct node *node)
{
	int sum = 0;

	if (node->desc1 == NULL) return(1);
	sum += count_desc(node->desc1);
	sum += count_desc(node->desc2);
	return(sum);
}