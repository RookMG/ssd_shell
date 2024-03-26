# 코드
각 폴더 안의 visual studio 프로젝트를 열면 보입니다.

# 실행파일
실행파일_Day2 폴더 안에 2일차 실행파일을 모아두었습니다.

### SSD
```
SSD.exe W {lba} {value}
SSD.exe R {lba}
SSD.exe E {lba} {size}
SSD.exe F
```

### Shell
Shell.exe 실행 후 help 명령어로 자세한 도움말을 확인 가능합니다.

### Runner
```
Runner.exe run_list.lst
```
콘솔에 각 테스트의 실행 결과가 출력되며, Shell의 출력이나 로그는 표시되지 않습니다.
중간에 테스트를 실패할 경우, 남은 테스트를 실행하지 않고 Runner가 종료됩니다.
[Runner]latest.log 혹은 [Runner]until_{time}.log , [Runner]until_{time}.zip 파일에 로그가 남습니다.
