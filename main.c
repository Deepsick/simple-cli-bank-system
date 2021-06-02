#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

const char STORE_FILE_NAME[] = "records.txt";
const char TEMP_STORE_FILE_NAME[] = "temp-records.txt";
const char RECORD_FORMAT[] = "%s %d %s %d %d %d %d";
const char DATE_FORMAT[] = "%d/%d/%d";

enum Choice
{
  CREATE = 1,
  UPDATE = 2,
  TRANSACTIONS = 3,
  GET = 4,
  DELETE = 5,
  GET_ALL = 6,
  EXIT = 7
};

enum Field
{
  NAME = 1,
  ACCOUNT_TYPE = 2
};

enum Transaction
{
  DEPOSIT = 1,
  WITHDRAW = 2
};

enum AccountTypes
{
  SAVING = 1,
  USUAL = 2
};
typedef struct
{
  int day, month, year;
} Date;


typedef struct
{
  char name[60];
  int acc_no;
  char acc_type[10];
  int balance;
  Date created_at;
  Date deposited_at;
  Date withdrawed_at;
} Account;

void close()
{
  exit(0);
}

Date enter_date(const char type[])
{
  printf("\n\n\nEnter %s date(mm/dd/yyyy): \n", type);
  int month, day, year;
  scanf(DATE_FORMAT, &month, &day, &year);
  if (month < 0 || month > 12)
  {
    return enter_date(type);
  }
  if (day < 0 || day > 31)
  {
    return enter_date(type);
  }
  if (year < 1500 || year > 2021)
  {
    return enter_date(type);
  }

  Date date = {day, month, year};
  return date;
}

void create(const char STORE_FILE_NAME[])
{
  static int acc_no = 1;
  FILE *file;
  file = fopen(STORE_FILE_NAME, "a+");
  if (file == NULL)
  {
    printf("fopen failed, errno = %d\n", errno);
  }
  Account account;
  account.acc_no = acc_no;
  acc_no++;
  account.balance = 0;
  Date created_at = enter_date("create");
  account.created_at = created_at;
  printf("Enter your name: \n");
  scanf("%s", account.name);
  printf("Enter your type. Available types: SAVING, USUAL \n");
  scanf("%s", account.acc_type);
  char format[22];
  strcpy(format, RECORD_FORMAT);
  strcat(format, "\n");
  fprintf(file, format, account.name, account.acc_no, account.acc_type, account.balance, account.created_at.day, account.created_at.month, account.created_at.year);
  fclose(file);
}

void change_value(Account *account, const int fieldChoice, const char value[])
{
  switch (fieldChoice)
  {
  case NAME:
    strcpy((*account).name, value);
    break;
  case ACCOUNT_TYPE:
    strcpy((*account).acc_type, value);
    break;
  default:
    break;
  }
}

void update(const char STORE_FILE_NAME[])
{
  FILE *old_file, *new_file;
  old_file = fopen(STORE_FILE_NAME, "r");
  new_file = fopen(TEMP_STORE_FILE_NAME, "w");
  if (old_file == NULL || new_file == NULL)
  {
    printf("fopen failed, errno = %d\n", errno);
  }
  printf("Your account number\n");
  int current_acc_no;
  scanf("%d", &current_acc_no);

  Account account;
  while (fscanf(old_file, RECORD_FORMAT, account.name, &account.acc_no, account.acc_type, &account.balance, &account.created_at.day, &account.created_at.month, &account.created_at.year) != EOF)
  {
    if (account.acc_no == current_acc_no)
    {
      printf("Enter what you want to update\n");
      printf("1 - Name\n");
      printf("2 - Account type\n");
      int choice;
      scanf("%d", &choice);
      switch (choice)
      {
      case NAME:
        printf("Enter new name: ");
        break;
      case ACCOUNT_TYPE:
        printf("Enter new account type(SAVING, USUAL).\n");
        break;
      default:
        printf("No such option");
      }
      char value[60];
      scanf("%s", value);
      change_value(&account, choice, value);
    }
    char format[22];
    strcpy(format, RECORD_FORMAT);
    strcat(format, "\n");
    fprintf(new_file, format, account.name, account.acc_no, account.acc_type, account.balance, account.created_at.day, account.created_at.month, account.created_at.year);
  }
  fclose(old_file);
  fclose(new_file);
  remove(STORE_FILE_NAME);
  rename(TEMP_STORE_FILE_NAME, STORE_FILE_NAME);
}

void get(const char STORE_FILE_NAME[])
{
  FILE *file;
  file = fopen(STORE_FILE_NAME, "r");
  if (file == NULL)
  {
    printf("Global error: %d\n", errno);
  }

  printf("Please, enter account number to get: \n");
  int account_no;
  scanf("%d", &account_no);

  Account account;
  while (fscanf(file, RECORD_FORMAT, account.name, &account.acc_no, account.acc_type, &account.balance, &account.created_at.day, &account.created_at.month, &account.created_at.year) != EOF)
  {
    if (account_no == account.acc_no)
    {
      printf(RECORD_FORMAT, account.name, account.acc_no, account.acc_type, account.balance, account.created_at.day, account.created_at.month, account.created_at.year);
      break;
    }
  }
  fclose(file);
}

void get_all(const char STORE_FILE_NAME[])
{
  FILE *file;
  file = fopen(STORE_FILE_NAME, "r");
  if (file == NULL)
  {
    printf("Global error: %d\n", errno);
  }

  Account account;
  char format[22];
  strcpy(format, RECORD_FORMAT);
  strcat(format, "\n");
  while (fscanf(file, RECORD_FORMAT, account.name, &account.acc_no, account.acc_type, &account.balance, &account.created_at.day, &account.created_at.month, &account.created_at.year) != EOF)
  {
    printf(format, account.name, account.acc_no, account.acc_type, account.balance, account.created_at.day, account.created_at.month, account.created_at.year);
  }
  fclose(file);
}

void handle_transactions(const char STORE_FILE_NAME[])
{
  FILE *old_file, *new_file;
  old_file = fopen(STORE_FILE_NAME, "r");
  new_file = fopen(TEMP_STORE_FILE_NAME, "w");
  if (old_file == NULL || new_file == NULL)
  {
    printf("Global error: %d\n", errno);
  }

  printf("Please, enter your account number: \n");
  int acc_no;
  scanf("%d", &acc_no);

  Account account;
  char format[22];
  strcpy(format, RECORD_FORMAT);
  strcat(format, "\n");
  while (fscanf(old_file, RECORD_FORMAT, account.name, &account.acc_no, account.acc_type, &account.balance, &account.created_at.day, &account.created_at.month, &account.created_at.year) != EOF)
  {
    if (account.acc_no == acc_no)
    {
      printf("What do you want to do?\n");
      printf("1 - Deposit\n");
      printf("2 - Withdraw\n");
      int choice;
      scanf("%d", &choice);
      int sum;
      switch (choice)
      {
      case DEPOSIT:
        printf("Your balance: %d\n", account.balance);
        printf("How much?\n");
        scanf("%d", &sum);
        if (sum < 0)
        {
          printf("Please, enter positive number\n");
          break;
        }

        account.balance += sum;
        printf("New balance: %d\n", account.balance);
        break;
      case WITHDRAW:
        if (strcmp(account.acc_type, "saving") == 0)
        {
          printf("You can't withdraw saving accounts\n");
          break;
        }
        printf("Your balance: %d\n", account.balance);
        printf("How much?\n");
        scanf("%d", &sum);
        if (account.balance - sum < 0)
        {
          printf("You can't withdraw more than your balance has\n");
          break;
        }
        account.balance -= sum;
        printf("New balance: %d\n", account.balance);
        break;
      default:
        printf("There is no such option");
        break;
      }
    }

    fprintf(new_file, format, account.name, account.acc_no, account.acc_type, account.balance, account.created_at.day, account.created_at.month, account.created_at.year);
  }

  fclose(old_file);
  fclose(new_file);
  remove(STORE_FILE_NAME);
  rename(TEMP_STORE_FILE_NAME, STORE_FILE_NAME);
}

void delete (const char STORE_FILE_NAME[])
{
  FILE *old_file, *new_file;
  old_file = fopen(STORE_FILE_NAME, "r");
  new_file = fopen(TEMP_STORE_FILE_NAME, "w");
  if (old_file == NULL || new_file == NULL)
  {
    printf("Global error: %d\n", errno);
  }

  printf("Please, enter account number to delete\n");
  int acc_no;
  scanf("%d", &acc_no);

  Account account;
  while (fscanf(old_file, RECORD_FORMAT, account.name, &account.acc_no, account.acc_type, &account.balance, &account.created_at.day, &account.created_at.month, &account.created_at.year) != EOF)
  {
    if (account.acc_no == acc_no)
    {
      continue;
    }
    char format[22];
    strcpy(format, RECORD_FORMAT);
    strcat(format, "\n");
    fprintf(new_file, format, account.name, account.acc_no, account.acc_type, account.created_at.day, account.created_at.month, account.created_at.year);
  }

  fclose(old_file);
  fclose(new_file);
  remove(STORE_FILE_NAME);
  rename(TEMP_STORE_FILE_NAME, STORE_FILE_NAME);
}

bool equals(const Account left, const Account right)
{
  return strcmp(left.name, right.name) && left.acc_no == right.acc_no && strcmp(left.acc_type, right.acc_type) && left.created_at.day == right.created_at.day && left.created_at.month == right.created_at.month && left.created_at.year == right.created_at.year;
}

void print_menu()
{
  printf("⚐⚐⚐ WELCOME TO THE MAIN MENU ⚐⚐⚐ \n\n");
  printf("1. Create a new account \n");
  printf("2. Update information of existing account \n");
  printf("3. For transactions \n");
  printf("4. Check the details of existing account \n");
  printf("5. Remove existing account \n");
  printf("6. View customer's list \n");
  printf("7. Exit \n");
}

int main(const int argc, char const *argv[])
{
  printf("CUSTOMER ACCOUNT BANKING MANAGEMENT SYSTEM \n\n\n");
  print_menu();

  printf("\n\n\n\n Enter your choice: \n");
  int choice;
  scanf("%d", &choice);
  switch (choice)
  {
  case CREATE:
    create(STORE_FILE_NAME);
    break;
  case UPDATE:
    update(STORE_FILE_NAME);
    break;
  case GET:
    get(STORE_FILE_NAME);
    break;
  case DELETE:
    delete (STORE_FILE_NAME);
    break;
  case GET_ALL:
    get_all(STORE_FILE_NAME);
    break;
  case TRANSACTIONS:
    handle_transactions(STORE_FILE_NAME);
    break;
  case EXIT:
    close();
    break;
  default:
    printf("There is no such option as %d", choice);
    exit(1);
  }

  return 0;
}
