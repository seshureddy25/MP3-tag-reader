# 🎵 MP3 Tag Reader

A C-based MP3 Tag Reader that extracts and displays metadata from MP3 audio files using the **ID3 tag format**. This project focuses on file handling, binary data processing, and understanding the structure of MP3 metadata.

## 📌 Features

* 🎧 Extract metadata from MP3 files.
* 🏷️ Read ID3 tag information.
* 📄 Display details such as:

  * Song Title
  * Artist
  * Album
  * Year
  * Genre
  * Comments
* 🔍 Detect ID3 tag versions supported by the implementation.
* 📂 Handle binary file operations to read metadata.
* ⚡ Efficient parsing of MP3 tag frames.

## 🛠️ Technologies Used

* **Language:** C
* **Concepts:**

  * File Handling
  * Structures
  * Pointers
  * Dynamic Memory Allocation
  * Bit Manipulation
  * Binary File Processing
  * String Manipulation

## 📁 Project Structure

```text
MP3-Tag-Reader/
│
├── main.c
├── read.c
├── read.h
├── types.h
├── common.h
├── Makefile
└── README.md
```

*Note: Adjust the file structure above to match your actual project files.*

## ⚙️ Compilation

Compile the project using GCC:

```bash
gcc *.c -o mp3_reader
```

Run the program:

```bash
./mp3_reader
```

## 🚀 Usage

1. Compile and execute the program.
2. Provide the MP3 file path if prompted.
3. The program reads the supported ID3 metadata.
4. View the extracted information in the terminal.

## 🧠 Key Learnings

* Understanding the internal structure of MP3 files.
* Working with ID3 metadata and frame identifiers.
* Reading and processing binary files in C.
* Using bitwise operations to interpret binary data.
* Improving memory management and debugging skills.

## 🔮 Future Improvements

* Support additional ID3 versions and frame types.
* Add an MP3 tag editor to modify metadata.
* Improve error handling for corrupted or unsupported files.
* Add support for Unicode text encoding.
* Build a more user-friendly command-line interface.

## 👨‍💻 Author

**Seshu Kumar Reddy**

* GitHub: [@seshureddy25](https://github.com/seshureddy25)
* LinkedIn: [Seshu Kumar Reddy](https://www.linkedin.com/in/seshu5/)

---

⭐ If you find this project interesting, feel free to explore the repository!
