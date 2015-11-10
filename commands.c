void cmd_reprogram()
{
    // TODO: obviously extract that address somewhere.
    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (void *)0x1E00;
    my_ptr();
}

int main(){cmd_reprogram(); return 0;}
