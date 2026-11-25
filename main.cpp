#include <algorithm>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace std;

// Struktur yang mengemas hasil perhitungan pemilihan barang.
struct KnapsackResult {
    // Jumlah nilai barang yang berhasil dimaksimalkan.
    long long value;
    // Jumlah berat semua barang yang dipilih.
    int totalWeight;
    // Indeks barang yang dipilih; indeks pertama dalam daftar adalah 0.
    vector<int> selectedItems;
};

// Menghitung pilihan barang bernilai terbesar yang tidak melebihi kapasitas tas.
// weight berisi berat barang, value berisi nilainya, dan capacity adalah batas berat tas.
KnapsackResult knapsackBottomUp(const vector<int>& weight, const vector<long long>& value, int capacity)
{
    // Kapasitas negatif tidak masuk akal untuk tas, jadi perhitungan dihentikan.
    if (capacity < 0) {
        throw invalid_argument("Kapasitas tas tidak boleh negatif.");
    }
    // Setiap barang harus memiliki satu data berat dan satu data nilai.
    if (weight.size() != value.size()) {
        throw invalid_argument("Jumlah berat dan nilai barang harus sama.");
    }
    // Berat nol atau negatif tidak diterima sebagai berat barang.
    for (int itemWeight : weight) {
        if (itemWeight <= 0) {
            throw invalid_argument("Berat setiap barang harus lebih dari 0.");
        }
    }

    // Mengubah jumlah barang menjadi bilangan bulat untuk dipakai dalam perulangan.
    const int itemCount = static_cast<int>(weight.size());
    // Tabel dp menyimpan nilai terbaik untuk sejumlah barang dan kapasitas tertentu.
    // Baris menunjukkan banyak barang yang dipertimbangkan; kolom menunjukkan kapasitas tas.
    vector<vector<long long>> dp(itemCount + 1, vector<long long>(capacity + 1, 0));

    // Mengisi tabel secara bertahap, mulai dari barang pertama hingga barang terakhir.
    for (int i = 1; i <= itemCount; ++i) {
        // Mencoba setiap kapasitas, dari 0 sampai kapasitas maksimum tas.
        for (int currentCapacity = 0; currentCapacity <= capacity;
             ++currentCapacity) {
            // Pilihan awal: barang saat ini tidak dimasukkan ke dalam tas.
            dp[i][currentCapacity] = dp[i - 1][currentCapacity];
            // Jika barang muat, bandingkan nilai tanpa barang dengan nilai jika barang diambil.
            if (weight[i - 1] <= currentCapacity) {
                dp[i][currentCapacity] = max(
                    dp[i][currentCapacity],
                    // Nilai barang ditambah hasil terbaik untuk kapasitas yang tersisa.
                    dp[i - 1][currentCapacity - weight[i - 1]] + value[i - 1]);
            }
        }
    }

    // Nilai di sudut kanan bawah tabel adalah nilai maksimal untuk semua barang dan kapasitas penuh.
    // Berat dan daftar pilihan mulai dari nol/kosong sebelum barang ditelusuri kembali.
    KnapsackResult result{dp[itemCount][capacity], 0, {}};
    // Kapasitas tersisa dipakai saat menelusuri tabel untuk mengetahui barang yang dipilih.
    int remainingCapacity = capacity;
    // Menelusuri tabel dari barang terakhir ke pertama untuk merekonstruksi pilihan.
    for (int i = itemCount; i > 0; --i) {
        // Jika nilai berubah dibanding baris sebelumnya, berarti barang ini ikut dipilih.
        if (dp[i][remainingCapacity] != dp[i - 1][remainingCapacity]) {
            // Simpan indeks barang (indeks dimulai dari 0).
            result.selectedItems.push_back(i - 1);
            // Tambahkan berat barang terpilih ke total berat.
            result.totalWeight += weight[i - 1];
            // Kurangi kapasitas tersisa untuk melanjutkan penelusuran.
            remainingCapacity -= weight[i - 1];
        }
    }
    // Penelusuran tadi berjalan mundur; balik daftar agar urut sesuai masukan.
    reverse(result.selectedItems.begin(), result.selectedItems.end());
    // Kirimkan seluruh hasil kepada bagian program yang memanggil fungsi ini.
    return result;
}

// Fungsi utama: titik awal program saat dijalankan.
int main()
{
    // Kesalahan masukan ditangani agar program dapat menampilkan pesan yang mudah dipahami.
    try {
        // itemCount menyimpan jumlah barang, sedangkan capacity menyimpan batas berat tas.
        int itemCount;
        int capacity;
        // Meminta pengguna mengisi jumlah barang dan kapasitas tas.
        cout << "Masukkan jumlah barang (N) dan kapasitas tas (W, kg): ";
        // Pastikan kedua masukan berhasil dibaca sebagai angka.
        if (!(cin >> itemCount >> capacity)) {
            throw invalid_argument("Input harus diawali jumlah barang dan kapasitas.");
        }
        // Jumlah barang tidak boleh kurang dari nol.
        if (itemCount < 0) {
            throw invalid_argument("Jumlah barang tidak boleh negatif.");
        }

        // Menyiapkan tempat untuk menyimpan berat dan nilai setiap barang.
        vector<int> weight(itemCount);
        vector<long long> value(itemCount);
        // Menyiapkan tempat untuk menyimpan nama barang.
        vector<string> name(itemCount);
        // Menjelaskan urutan data yang perlu dimasukkan pengguna.
        cout << "Masukkan data barang dengan format: berat(kg) nilai nama barang.\n";
        // Membaca data barang satu per satu.
        for (int i = 0; i < itemCount; ++i) {
            // Memberi tahu pengguna barang keberapa yang sedang dimasukkan.
            cout << "Barang ke-" << i + 1
                 << " (berat dalam kg, nilai, lalu nama): ";
            // Berat dan nilai dibaca sebagai angka terlebih dahulu.
            if (!(cin >> weight[i] >> value[i])) {
                throw invalid_argument("Data berat dan nilai barang tidak lengkap.");
            }
            // Membaca sisa baris sebagai nama, sehingga nama boleh terdiri dari beberapa kata.
            getline(cin, name[i]);
            // Mencari posisi karakter nama pertama yang bukan spasi atau tab.
            const size_t firstCharacter = name[i].find_first_not_of(" \t");
            // Jika nama kosong atau hanya spasi, gunakan nama bawaan berdasarkan nomor barang.
            if (firstCharacter == string::npos) {
                name[i] = "Barang ke-" + to_string(i + 1);
            } else {
                // Jika ada nama, buang spasi/tab yang berada di bagian awal nama.
                name[i].erase(0, firstCharacter);
            }
        }

        // Menghitung kombinasi barang terbaik berdasarkan data yang sudah dimasukkan.
        const KnapsackResult result = knapsackBottomUp(weight, value, capacity);
        // Menampilkan jumlah nilai maksimal yang bisa dibawa.
        cout << "Nilai maksimal: " << result.value << '\n';
        // Menampilkan berat gabungan barang yang dipilih.
        cout << "Total berat: " << result.totalWeight << " kg\n";
        // Menampilkan banyaknya barang terpilih.
        cout << "Jumlah barang terbawa: " << result.selectedItems.size() << " barang\n";
        // Menjadi awalan untuk daftar nama barang pilihan.
        cout << "Barang terpilih:";
        // Jika daftar pilihan kosong, beri tahu pengguna.
        if (result.selectedItems.empty()) {
            cout << " tidak ada\n";
        } else {
            // Menampilkan nama, berat, dan nilai setiap barang yang dipilih.
            for (int index : result.selectedItems) {
                cout << " [" << name[index] << ", " << weight[index] << " kg, nilai " << value[index] << "]";
            }
            // Pindah ke baris baru setelah daftar selesai ditampilkan.
            cout << '\n';
        }
    // Menangkap kesalahan validasi dan menampilkan penjelasannya.
    } catch (const invalid_argument& error) {
        cerr << "Error: " << error.what() << '\n';
        // Kode 1 menandakan program berakhir karena terjadi kesalahan.
        return 1;
    }
    // Kode 0 menandakan program selesai dengan normal.
    return 0;
}
