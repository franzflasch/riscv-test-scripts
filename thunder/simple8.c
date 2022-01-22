int _start(int x)
{

    begin:
    if(x == 42)
        return 1;
    else if(x==43)
        return 5;
    goto begin;

    return 0;
}
