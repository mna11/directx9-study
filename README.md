# DirectX 9 Study
<img width="500" height="625" alt="Image" src="https://github.com/user-attachments/assets/c2cd6076-1282-41b6-bd57-55c62f2c3f29" />

프랭크 루나의 《DirectX 9를 이용한 3D GAME 프로그래밍 입문》(용책) 예제를 따라 정리한 C++ 실습 코드입니다. 학습 과정에서 일부 구현을 수정했으며, 예제는 챕터별 브랜치로 정리했습니다.

## 챕터별 예제

1장의 Direct3D 초기화 코드는 `main`에 있습니다. 2장의 렌더링 파이프라인은 이후 실습에 적용했으며, 별도 예제 브랜치는 없습니다.

| 브랜치 | 예제 |
| --- | --- |
| `chapter-3` | 버텍스·인덱스 버퍼를 사용한 삼각형, 큐브, 주전자 그리기 |
| `chapter-4` | 정점 색과 플랫·고로 셰이딩 비교 |
| `chapter-5` | 피라미드의 법선, 재질, 조명 |
| `chapter-5-light` | 방향광, 점광원, 스포트라이트 |
| `chapter-6` | 사각형에 텍스처 입히기 |
| `chapter-6-address-mode` | 텍스처 어드레스 모드 |
| `chapter-6-cube` | 큐브의 각 면에 텍스처 입히기 |
| `chapter-7` | 재질의 알파값을 이용한 투명 렌더링 |
| `chapter-7-tex-alpha` | 텍스처의 알파 채널을 이용한 블렌딩 |
| `chapter-8` | 스텐실 버퍼를 이용한 거울 반사 |
| `chapter-8-shadow` | 평면 그림자와 스텐실 테스트 |
| `chapter-9` | `ID3DXFont`로 FPS 표시 |
| `chapter-9-cd3dfont` | `CD3DFont`로 텍스트 그리기 |
| `chapter-9-create-text` | `D3DXCreateText`로 3D 글자 메시 만들기 |
| `chapter-10` | 메시 생성과 서브셋 렌더링 |
| `chapter-11` | `.x` 메시, 재질, 텍스처 불러오기 |
| `chapter-11-bounding` | 메시의 경계 볼륨 계산과 표시 |
| `chapter-11-progressive` | 프로그레시브 메시의 세부 수준 조절 |
| `chapter-12` | 이동과 회전이 가능한 카메라 |
| `chapter-13` | 높이맵으로 지형 생성 및 렌더링 |
| `chapter-14` | 눈, 불꽃놀이, 파티클 총 |
| `chapter-15` | 피킹 광선을 이용한 마우스 선택 |
| `chapter-16` | HLSL로 정점 위치 변환 |
| `chapter-17` | 버텍스 셰이더로 확산광 계산 |
| `chapter-17-toon` | 툰 셰이딩과 실루엣 외곽선 |
| `chapter-18` | 픽셀 셰이더로 기본·조명·문자 텍스처 합성 |
| `chapter-19` | 이펙트 파일로 고정 기능 조명과 텍스처 설정 |
| `chapter-19-fog` | 이펙트 파일로 지형에 선형 안개 적용 |
| `chapter-19-toon` | 이펙트 프레임워크로 툰 셰이딩 구현 |

## 빌드 및 실행

1. Visual Studio 2022의 C++ 데스크톱 개발 도구와 DirectX SDK (June 2010)를 설치합니다.
2. 실행할 예제 브랜치로 전환합니다. 예를 들어 `git switch chapter-19-toon`을 실행합니다.
3. `DX9/DX9.sln`을 열고 `Debug | x64` 구성으로 빌드 및 실행합니다.

리소스를 사용하는 예제의 실행 작업 디렉터리는 `DX9`입니다.