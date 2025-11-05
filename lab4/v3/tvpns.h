int NUMSESSIONS = 8;
struct tunneltab {
   struct in6_addr destaddr;
   unsigned short destpt;
   struct in6_addr sourceaddr;
   unsigned short sourcept;
   unsigned short tunnelpt; };


