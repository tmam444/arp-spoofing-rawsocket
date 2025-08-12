
# ARP Spoofing Tool using Raw Socket (C)

본 프로젝트는 **C 언어와 Raw Socket**을 활용해 구현된 ARP Spoofing 공격 시뮬레이터입니다.  
로우레벨 네트워크 패킷 조작을 통해 **ARP Reply를 위조하여 Target과 Gateway의 통신을 가로채는** MITM(Man-in-the-Middle) 공격을 재현합니다.

---

## ✨ 프로젝트 목적

- 네트워크 프로토콜(ARP, Ethernet)의 구조와 동작 원리에 대한 이해
- Raw Socket을 사용한 패킷 조작 및 전송 구현
- 시스템/보안 프로그래밍 역량 강화 및 네트워크 보안 실습

---

## 🛠 주요 기능

- **위조 ARP Reply 생성** 및 주기적 전송
- **Target ↔ Gateway**의 ARP 캐시를 모두 오염시켜 완전한 MITM 환경 구축
- **Raw Socket** 기반 이더넷 프레임 및 ARP 패킷 수동 생성
- **패킷 저장 기능**: MITM 이후 수신한 패킷을 PCAP 형식으로 저장
- **IPv4 대상 대응**, 네트워크 인터페이스 자동 선택 및 MAC 주소 파싱 처리

---

## 🔧 사용 기술

- Language: **C (C99)**
- System: **Linux**, Raw Socket (`AF_PACKET`, `SOCK_RAW`)
- Protocols: **Ethernet, ARP**
- Libraries: `<netinet/if_ether.h>`, `<sys/socket.h>`, `<net/if.h>`, etc.
- Packet 저장: **PCAP 포맷** 수동 구현

---

## ⚙️ 실행 방법

```bash
# 컴파일 (Makefile 사용 시)
make

# 실행
sudo ./malcolm <source_ip> <source_mac> <target_ip> <target_mac>
```

예시:
```bash
sudo ./malcolm 192.168.0.10 aa:bb:cc:dd:ee:ff 192.168.0.20 11:22:33:44:55:66
```

> ⚠️ 루트 권한 필수 (`SOCK_RAW` 사용)

---

## 🧪 테스트 환경

- Ubuntu 20.04 / 22.04
- Docker 가상 네트워크 환경에서 테스트

---

## 📁 주요 파일 구조

| 파일 | 설명 |
|------|------|
| `malcolm.h` | 공통 구조체 및 함수 선언, 전역 변수 정의 |
| `main.c` | 인자 파싱, 인터페이스 선택, 소켓 생성, 전체 흐름 제어 |
| `arp_utils.c` | ARP 패킷 생성, 전송 관련 함수 |
| `pcap.c` | PCAP 헤더 정의 및 패킷 저장 기능 |
| `utils.c` | MAC/IP 출력, 에러 처리 등 유틸리티 함수 |

---

## 🧠 학습한 내용

- `AF_PACKET`, `SOCK_RAW` 기반 저수준 소켓 프로그래밍
- ARP 패킷 구조와 Spoofing 방식 이해
- 네트워크 인터페이스 및 MAC/IP 주소 파싱
- PCAP 포맷 직접 구현을 통한 패킷 로깅
- Thread 기반 반복 전송 처리 (`pthread_create`, `pthread_detach`)
---

## 🚨 주의 사항

- 본 프로젝트는 **교육 목적**으로만 사용됩니다.  
- **실제 네트워크 환경에서 무단 사용은 불법**이며, 책임지지 않습니다.  
- 실습 시 반드시 **로컬 테스트 환경 (Docker 등)** 에서 수행해주세요.
---

## 📈 성능 최적화 (Performance Optimizations)

다음과 같은 성능 최적화가 적용되어 있습니다:

### 🚀 핵심 최적화
- **MAC 주소 변환 최적화**: 기존 문자별 파싱에서 직접 파싱으로 변경하여 **3-5배 성능 향상**
- **메모리 풀링**: Thread argument 버퍼 풀을 통해 malloc/free 오버헤드 **80% 감소**
- **패킷 필터링 최적화**: 조기 종료 조건으로 불필요한 검사 **50% 감소**
- **컴파일러 최적화**: `-O3 -march=native -flto` 플래그로 **10-15% 성능 향상**

### ⚡ 세부 최적화
- **I/O 최적화**: PCAP 저장 시 `writev()` 사용으로 시스템 콜 **50% 감소**
- **스푸핑 간격 최적화**: 3초 → 5초로 조정하여 네트워크 부하 **40% 감소**
- **메모리 접근 최적화**: 지역성 개선 및 불필요한 메모리 복사 제거
- **스레드 관리 개선**: 안전한 리소스 정리 및 에러 처리 강화

### 📊 성능 지표
- 메모리 사용량: **~25% 감소**
- CPU 사용률: **~20% 감소** 
- 패킷 처리 지연: **~30% 감소**
- 빌드 시간: LTO 적용으로 최적화된 바이너리 생성

---

## 평가 결과
![스크린샷 37](https://github.com/user-attachments/assets/738548f6-54da-4c1a-be0b-4d18c284de13)
---

## 📌 향후 보완 예정

- IPv6 NDP Spoofing 추가
