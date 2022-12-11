// #include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
// #include <unistd.h>

#define SYSCALL_HELLO 449

// int main()
// {
//     long int amma = syscall(SYSCALL_HELLO, getpid());
//     printf("System call sys_hello returend %ld\n", amma);
//     return 0;
// }


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 子執行緒函數
void* child(void* data) {
  char *str = (char*) data; // 取得輸入資料
  for(int i = 0;i < 3;++i) {
    printf("%s\n", str); // 每秒輸出文字
    sleep(1);
  }
  long int amma = syscall(SYSCALL_HELLO, getpid());
printf("System call sys_hello returend %ld\n", amma);
  pthread_exit(NULL); // 離開子執行緒
}

// 主程式
int main() {
  pthread_t t; // 宣告 pthread 變數
  pthread_create(&t, NULL, child, "Child"); // 建立子執行緒
long int amma = syscall(SYSCALL_HELLO, getpid());
    printf("System call sys_hello returend %ld\n", amma);
  // 主執行緒工作
  for(int i = 0;i < 3;++i) {
    printf("Master\n"); // 每秒輸出文字
    sleep(1);
  }

  pthread_join(t, NULL); // 等待子執行緒執行完成
  return 0;
}