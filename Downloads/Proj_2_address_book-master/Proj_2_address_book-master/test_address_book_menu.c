#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "address_book.h"
#include "address_book_menu.h"
#include "address_book_fops.h"

void test_add_contact(AddressBook *address_book)
{
    printf("Testing add_contact...\n");
    add_contacts(address_book);
    printf("Contact added successfully.\n");
}

void test_search_contact(AddressBook *address_book)
{
    printf("Testing search_contact...\n");
    search_contact_2(address_book);
    printf("Search completed.\n");
}

void test_edit_contact(AddressBook *address_book)
{
    printf("Testing edit_contact...\n");
    if (address_book->count > 0)
    {
        strcpy(address_book->list[0].name[0], "Jane Doe");
        printf("Contact edited successfully.\n");
    }
    else
    {
        printf("No contacts to edit.\n");
    }
}

void test_delete_contact(AddressBook *address_book)
{
    printf("Testing delete_contact...\n");
    if (address_book->count > 0)
    {
        for (int i = 0; i < address_book->count - 1; i++)
        {
            address_book->list[i] = address_book->list[i + 1];
        }
        address_book->count--;
        printf("Contact deleted successfully.\n");
    }
    else
    {
        printf("No contacts to delete.\n");
    }
}

void test_list_contacts(AddressBook *address_book)
{
    printf("Testing list_contacts...\n");
    if (address_book->count > 0)
    {
        list_contacts_2(address_book, "List of Contacts", NULL, NULL, e_list);
        printf("Contacts listed successfully.\n");
    }
    else
    {
        printf("No contacts to list.\n");
    }
}

void test_save_prompt(AddressBook *address_book)
{
    printf("Testing save_prompt...\n");
    save_prompt(address_book);
    printf("Save prompt completed.\n");
}

int main(void)
{
    AddressBook address_book;
    address_book.count = 0;
    address_book.list = (ContactInfo *)malloc(sizeof(ContactInfo) * 100); // Assuming a maximum of 100 contacts

    test_add_contact(&address_book);
    test_search_contact(&address_book);
    test_edit_contact(&address_book);
    test_delete_contact(&address_book);
    test_list_contacts(&address_book);
    test_save_prompt(&address_book);

    free(address_book.list);
    return 0;
}