#include <stdio.h>
//#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "address_book.h"
#include "address_book_menu.h"
//#include "address_book_fops.h"

//#include "abk_fileops.h"
//#include "abk_log.h"
//#include "abk_menus.h"
//#include "abk.h"

int get_option(int type, const char *msg)
{
	/*
	 * Mutilfuction user intractions like
	 * Just an enter key detection
	 * Read an number
	 * Read a charcter
	 */ 

	/* Fill the code to add above functionality */
	char input[32];
    int option = -1;

    printf("%s", msg);
    if (fgets(input, sizeof(input), stdin) == NULL)
	{
		return -1; // Handle EOF or input error
	}

    if (type == NUM)
    {
        option = atoi(input);
    }
    else if (type == CHAR)
    {
        option = input[0];
    }

    return option;
}

Status save_prompt(AddressBook *address_book)
{
	char option;

	do
	{
		main_menu();

		option = get_option(CHAR, "\rEnter 'N' to Ignore and 'Y' to Save: ");

		if (option == 'Y')
		{
			save_file(address_book);
			printf("Exiting. Data saved in %s\n", DEFAULT_FILE);
			system("pause");
			break;
		}
		else if (option == 'N')
		{
			printf("Exiting without saving.\n");
			break;
		}
		else
		{
			printf("Invalid option. Please enter 'Y' or 'N'.\n");
		}

	} while (option != 'N');

	free(address_book->list);

	return e_success;
}

Status list_contacts_2(AddressBook *address_book, const char *title, int *index, const char *msg, Modes mode)
{
	int page = 0;
	int option = 1;

	do
	{
		menu_header(title); // Use updated menu_header for screen clearing
		print_pattern();
		printf(": %6s : %32s : %32s : %32s :\n", "S.No", "Name", "Phone No.", "Email ID");
		print_pattern();

		for (int i = page * 5; i < (page + 1) * 5 && i < address_book->count; i++)
		{
			printf(": %6d : %32s : %32s : %32s :\n",
			       address_book->list[i].si_no,
			       address_book->list[i].name[0],
			       address_book->list[i].phone_numbers[0],
			       address_book->list[i].email_addresses[0]);
		}

		print_pattern();
		printf("Page %d of %d\n", page + 1, (address_book->count + 4) / 5);
		printf("Options: [0] Exit | [1] Next Page | [2] Previous Page\n");

		option = get_option(NUM, "Enter your choice: ");
		switch (option)
		{
			case 0:
				return e_success;
			case 1:
				if ((page + 1) * 5 < address_book->count)
					page++;
				else
					printf("No more pages.\n");
				break;
			case 2:
				if (page > 0)
					page--;
				else
					printf("Already on the first page.\n");
				break;
			default:
				printf("Invalid option. Try again.\n");
		}
        system("pause");
    } while (option != 0);
	return e_fail;
}

void print_pattern()
{
for(int a = 0; a < 115; a++)
       {
       printf("=");
       }
       printf("\n");
}

void menu_header(const char *str)
{
	fflush(stdout);

	system("cls");

	printf("#######  Address Book  #######\n");
	if (str != NULL)
	{
		printf("#######  %s\n", str);
	}

}

void main_menu(void)
{
	menu_header("Features:\n");

	printf("0. Exit\n");
	printf("1. Add Contact\n");
	printf("2. Search Contact\n");
	printf("3. Edit Contact\n");
	printf("4. Delete Contact\n");
	printf("5. List Contacts\n");
	printf("6. Save\n");
	printf("\n");
	printf("Please select an option: ");
}

Status menu(AddressBook *address_book)
{
	ContactInfo backup;
	Status ret;
	int option;

	do
	{
		main_menu();

		option = get_option(NUM, "");

		if ((address_book-> count == 0) && (option != e_add_contact))
		{
			get_option(NONE, "No entries found!!. Would you like to add? Use Add Contacts");

			continue;
		}

		switch (option)
		{
			case e_add_contact:
				/* Add your implementation to call add_contacts function here */
				add_contacts(address_book);
				break;
			case e_search_contact:
				search_contact_2(address_book);
				break;
			case e_edit_contact:
				edit_contact_2(address_book);
				break;
			case e_delete_contact:
				delete_contact_2(address_book);
				break;
			case e_list_contacts:
				/* Add your implementation to call list_contacts function here */
				//list_contacts(address_book, "List of Contacts", NULL, "Press any key to continue...", e_list_contacts);
				list_contacts_2(address_book, "List of Contacts", NULL, NULL, e_list);
				break;
			case e_save:
				save_file(address_book);
				break;
			case e_exit:
				exit(0);
				break;
		}

		system("pause");
		system("cls");

	} while (option != e_exit);

	return e_success;
}

Status add_contacts(AddressBook *address_book)
{
	/* Add the functionality for adding contacts here */
	if (address_book->count >= 100) // Assuming a maximum of 100 contacts
    {
        printf("Address book is full!\n");
        return e_fail;
    }

    ContactInfo new_contact;
    printf("Enter Name: ");
    fgets(new_contact.name[0], NAME_LEN, stdin);
    new_contact.name[0][strcspn(new_contact.name[0], "\n")] = '\0'; // Remove newline character

    printf("Enter Phone Number: ");
    fgets(new_contact.phone_numbers[0], NUMBER_LEN, stdin);
    new_contact.phone_numbers[0][strcspn(new_contact.phone_numbers[0], "\n")] = '\0'; // Remove newline character

    printf("Enter Email Address: ");
    fgets(new_contact.email_addresses[0], EMAIL_ID_LEN, stdin);
    new_contact.email_addresses[0][strcspn(new_contact.email_addresses[0], "\n")] = '\0'; // Remove newline character

    new_contact.si_no = address_book->count + 1;
    address_book->list[address_book->count] = new_contact;
    address_book->count++;

	printf("Contact added successfully!\n");
    return e_success;
}

Status search(const char *str, AddressBook *address_book, int loop_count, int field, const char *msg, Modes mode)
{
	char search_str[NAME_LEN];
	int option, count = 0;
 
	do
	{
	    menu_header("Search Contact");
 
	    printf("Choose search criteria:\n");
	    printf("1. Search by Name\n2. Search by Phone No\n3. Search by Email ID\n4. Back to Menu\n");
	    option = get_option(NUM, "Enter your option: ");
 
	    switch (option)
	    {
		   case 1:
			  printf("Enter Name to search: ");
			  fgets(search_str, NAME_LEN, stdin);
			  search_str[strcspn(search_str, "\n")] = '\0'; 
			  count = search(search_str, address_book, address_book->count, 0, NULL, e_search);
			  break;
 
		   case 2:
			  printf("Enter Phone No to search: ");
			  fgets(search_str, NUMBER_LEN, stdin);
			  search_str[strcspn(search_str, "\n")] = '\0';
			  count = search(search_str, address_book, address_book->count, 1, NULL, e_search);
			  break;
 
		   case 3:
			  printf("Enter Email ID to search: ");
			  fgets(search_str, EMAIL_ID_LEN, stdin);
			  search_str[strcspn(search_str, "\n")] = '\0';
			  count = search(search_str, address_book, address_book->count, 2, NULL, e_search);
			  break;
 
		   case 4:
			  return e_success; // Exit search menu
 
		   default:
			  printf("Invalid option. Please try again.\n");
	    }
 
	    if (count == 0)
	    {
		   printf("No matching contacts found.\n");
	    }
 
	    system("pause");
	} while (option != 4);
 
	return e_success;
 }

Status search_contact_2(AddressBook *address_book)
{
	/* Add the functionality for search contacts here */
	// Add the functionality for searching contacts here
    // Example:
    char search_str[NAME_LEN];
    printf("Enter name to search: ");
    fgets(search_str, NAME_LEN, stdin);
    search_str[strcspn(search_str, "\n")] = '\0'; // Remove newline character

    return search(search_str, address_book, address_book->count, 0, NULL, e_search);
}

Status edit_contact_2(AddressBook *address_book)
{
	/* Add the functionality for edit contacts here */
    
	int index;
    printf("Enter contact index to edit: ");
    scanf("%d", &index);
    getchar(); // Consume newline character

    if (index < 1 || index > address_book->count)
    {
        printf("Invalid index!\n");
        return e_fail;
    }

    ContactInfo *contact = &address_book->list[index - 1];
    printf("Editing contact: %s\n", contact->name[0]);

    printf("Enter new Name (leave blank to keep current): ");
    char new_name[NAME_LEN];
    fgets(new_name, NAME_LEN, stdin);
    if (new_name[0] != '\n')
    {
        new_name[strcspn(new_name, "\n")] = '\0'; // Remove newline character
        strcpy(contact->name[0], new_name);
    }

    printf("Enter new Phone Number (leave blank to keep current): ");
    char new_phone[NUMBER_LEN];
    fgets(new_phone, NUMBER_LEN, stdin);
    if (new_phone[0] != '\n')
    {
        new_phone[strcspn(new_phone, "\n")] = '\0'; // Remove newline character
        strcpy(contact->phone_numbers[0], new_phone);
    }

    printf("Enter new Email Address (leave blank to keep current): ");
    char new_email[EMAIL_ID_LEN];
    fgets(new_email, EMAIL_ID_LEN, stdin);
    if (new_email[0] != '\n')
    {
        new_email[strcspn(new_email, "\n")] = '\0'; // Remove newline character
        strcpy(contact->email_addresses[0], new_email);
    }

    return e_success;
	
   int option;

	do 
	{
		// Print edit menu
		menu_header("Search Contact to Edit by:\n");

		printf("0. Back\n");
		printf("1. Name\n");
		printf("2. Phone No\n");
		printf("3. Email ID\n");
		printf("4. Serial No\n");
		printf("\n");
		// End of edit menu
		option = get_option(NUM, "Please select an option: ");
		
		int book_length = address_book->count;
		int info_length;
		char * search_pattern;
		
		printf("Enter a pattern: "); // TODO: replace pattern with name of search method
		scanf("%s", search_pattern);
	
		for (int i = 0; i < book_length; i++) 
		{
			ContactInfo * contact = &address_book->list[i];

			switch (option) // search by option selected
			{
				case 0: // back
					continue;	
				case 1: // name
					if (strcmp(search_pattern, contact->name[0]) == 0)
					{
						edit_print_result(address_book, i);
					}
					break;
				case 2: // phone no 
					for (int j = 0; j < PHONE_NUMBER_COUNT; j++) {
						if (strcmp(search_pattern, contact->phone_numbers[j]) == 0)
						{
							edit_print_result(address_book, i);
						}
					}
					break;
				case 3: // email id 
					for (int j = 0; j < EMAIL_ID_COUNT; j++) {
						if (strcmp(search_pattern, contact->email_addresses[j]) == 0)
						{
							edit_print_result(address_book, i);
						}
					}
					break;
				case 4: // serial no 
					if (atoi(search_pattern) == contact->si_no)
					{
						edit_print_result(address_book, i);
					}
					break;
				default:
					printf("Invalid input.\n");
					continue;
				}
		}

		// Editor options
		char select_option;

		do
		{
			select_option = get_option(CHAR, "Press [s] = Select. [q] | Cancel: ");

			if (select_option == 's') 
			{
				int serial_selection = get_option(NUM, "Select a Serial Number (S.No) to Edit: ");
		
				/*
					Check for invalid serial numbers	
				*/				

				int index;
				for (index = 0; index < address_book->count; index++)
				{ // NOTE: May not be necessary if si_no corresponds to its list index
					if (serial_selection == address_book->list[index].si_no)	
					{
						break;
					}
				}

				// Edit contact menu
				menu_header("Edit Contact:\n");
				
				printf("0. Back\n");
				printf("1. Name : %s\n", address_book->list[index].name[0]);
				printf("2. Phone No ");
				for (int i = 0; i < PHONE_NUMBER_COUNT; i++)
				{
					printf("%d : %s\n", 
						i, address_book->list[index].phone_numbers[i]);
				}
				printf("3. Email Id ");
				for (int i = 0; i < EMAIL_ID_COUNT; i++)
				{
					printf("%d : %s\n", 
						i, address_book->list[index].email_addresses[i]);
				}
				printf("\n");
				// End of edit contact menu

				int edit_option;
				int edit_index;
				char * edit_changes;

				do
				{
					edit_option = get_option(NUM, "Please select an option: ");

					switch (edit_option)
					{
						case 0: // Back
							continue;	
						case 1: // Name
							printf("Enter name: ");
							scanf("%s", edit_changes);
							strcpy(address_book->list[index].name[0], edit_changes);
							break;
						case 2: // Phone no
							do
							{
								edit_index = get_option(NUM, "Enter phone no index to be changed [Max 5]: ");
								if (edit_index <= 0 || edit_index >= 5)
								{
									printf(" | Invalid index. Try again\n");
								}
							} while (edit_index <= 0 || edit_index >= 5);

							printf("Enter phone no %d: ", edit_index);
							scanf("%s", edit_changes);
							strcpy(address_book->list[index].phone_numbers[edit_index], edit_changes);
							break;
						case 3: // Email id
							do
							{
								edit_index = get_option(NUM, "Enter email id index to be changed [Max 5]: ");
								if (edit_index <= 0 || edit_index >= 5)
								{
									printf(" | Invalid index. Try again\n");
								}
							} while (edit_index <= 0 || edit_index >= 5);

							printf("Enter email id %d: ", edit_index);
							scanf("%s", edit_changes);
							strcpy(address_book->list[index].email_addresses[edit_index], edit_changes);
							break;
						default:
							printf("Invalid input. Try again\n");
							continue;
					}
					
					} while (edit_option != 0);
			
			}
			else if (select_option != 'q') 
			{
				printf("Invalid input. Try again\n");
			}

		} while (select_option != 's' || select_option != 'q');
		
	} while (option != e_back);

	return e_success;
}

Status delete_contact_2(AddressBook *address_book)
{
	/* Add the functionality for delete contacts here */

	int index;
    printf("Enter contact index to delete: ");
    scanf("%d", &index);
    getchar(); // Consume newline character

    if (index < 1 || index > address_book->count)
    {
        printf("Invalid index!\n");
        return e_fail;
    }

    for (int i = index - 1; i < address_book->count - 1; i++)
    {
        address_book->list[i] = address_book->list[i + 1];
    }
    address_book->count--;

    return e_success;
}

void edit_print_result(AddressBook *address_book, int index)
{
	printf("%d | ", address_book->list[index].si_no);
	for (int i = 0; i < 5; i++) // 5 is the max length	
	{ 
		if (i != 0) { printf("\t"); }
		printf("%s | ", address_book->list[index].name[i]);
		printf("%s | ", address_book->list[index].phone_numbers[i]);
		printf("%s", address_book->list[index].email_addresses[i]);
	}
}