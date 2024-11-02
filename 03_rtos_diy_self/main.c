void delay(int count)
{
	while(count-- > 0);
}

int flag;

int main()
{
	while(1)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
	}
	
	return 0;
}
