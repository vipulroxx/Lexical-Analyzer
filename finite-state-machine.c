#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum
{
	start, build_id, build_num, identifier, number, stop
}state;

state transition(state current_state, char transition_char)
{
	state next_state;
	switch (current_state)
	{
		case start:
			if (isalpha(transition_char))
				next_state = build_id;
			if (isdigit(transition_char))
				next_state = build_num;
			if (transition_char == ' ')
				next_state = start;
			if (transition_char == '.')
				next_state = stop;
			break;
		case number:
			next_state = start;
			break;
		case build_id:
			if (isalpha(transition_char) || isdigit(transition_char) || transition_char == '_')
				next_state = build_id;
			if (transition_char == ' ')
				next_state = identifier;
			break;
		case identifier:
			next_state = start;
			break;
		case build_num:
			if (isdigit(transition_char))
				next_state = build_num;
			if (transition_char == ' ')
				next_state = number;
			break;
		case stop:
			next_state = stop;
			break;
	}
	return next_state;
}

int main()
{
	state current_state;
	char transition_char;
	current_state = start;
	do
	{
		if (current_state == identifier)
		{
			printf(" - Identifier \n");
			current_state = start;
		}
		else if (current_state == number)
		{
			printf(" - Number\n");
			current_state = start;
		}
		scanf("%c", &transition_char);
		if (transition_char != ' ')
			printf("%c", transition_char);
		current_state = transition(current_state, transition_char);
	} while (current_state != stop);
	return 0;
}
