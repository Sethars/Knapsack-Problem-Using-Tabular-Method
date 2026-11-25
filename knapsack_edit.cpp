/**
 * @file knapsack.cpp
 * @brief Penyelesaian masalah 0/1 Knapsack dengan Dynamic Programming (bottom-up).
 *
 * Program membaca daftar barang (berat, nilai, nama) dan kapasitas tas,
 * lalu menentukan kombinasi barang dengan total nilai maksimal tanpa
 * melebihi kapasitas. Setiap barang hanya dapat dipilih maksimal satu kali.
 *
 * Format input:
 * @code
 * N W
 * berat_1 nilai_1 nama_1
 * ...
 * berat_N nilai_N nama_N
 * @endcode
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace std;

/**
 * @brief Hasil perhitungan 0/1 Knapsack.
 */
struct KnapsackResult {
    /** Total nilai maksimal yang dapat diperoleh. */
    long long value;

    /** Total berat barang yang terpilih dalam kg. */
    int totalWeight;

    /** Indeks (mulai dari 0) barang yang terpilih, terurut menaik. */
    vector<int> selectedItems;
};

/**
 * @brief Menyelesaikan 0/1 Knapsack dengan pendekatan bottom-up.
 *
 * Tabel `dp[i][c]` menyimpan nilai maksimal yang dapat diperoleh dari
 * `i` barang pertama dengan kapasitas `c`. Relasi rekurensinya:
 * - Barang tidak diambil: `dp[i-1][c]`
 * - Barang diambil (jika muat): `dp[i-1][c - weight[i-1]] + value[i-1]`
 *
 * `dp[i][c]` adalah nilai terbesar di antara kedua pilihan tersebut.
 * Setelah tabel terisi, barang terpilih ditelusuri mundur (backtracking).
 * Jika ada beberapa solusi dengan nilai sama, barang berindeks lebih besar
 * tidak diambil selama nilai maksimal tetap tercapai tanpanya.
 *
 * @param weight   Berat setiap barang dalam kg; semua harus lebih dari 0.
 * @param value    Nilai setiap barang; urutannya sesuai dengan @p weight.
 *                 Jumlah seluruh nilai harus muat dalam `long long`.
 * @param capacity Kapasitas maksimal tas dalam kg; tidak boleh negatif.
 *
 * @return Nilai maksimal, total berat, dan indeks barang yang terpilih.
 *
 * @throws std::invalid_argument Jika @p capacity negatif, ukuran @p weight
 *         dan @p value berbeda, atau ada berat yang tidak lebih dari 0.
 * @throws std::bad_alloc Jika memori tidak cukup untuk tabel DP.
 *
 * @note Kompleksitas waktu O(N * W) dan ruang O(N * W), dengan N jumlah
 *       barang dan W kapasitas tas.
 */
KnapsackResult knapsackBottomUp(const vector<int>& weight, const vector<long long>& value, int capacity) {
    if (capacity < 0) {
        throw invalid_argument("Kapasitas tas tidak boleh negatif.");
    }
    if (weight.size() != value.size()) {
        throw invalid_argument("Jumlah berat dan nilai barang harus sama.");
    }
    for (int itemWeight : weight) {
        if (itemWeight <= 0) {
            throw invalid_argument("Berat setiap barang harus lebih dari 0.");
        }
    }

    const int itemCount = static_cast<int>(weight.size());
    // Inisialisasi tabel dp dengan baris 0 (tanpa barang) dan kolom 0 (kapasitas nol) bernilai 0.
    vector<vector<long long>> dp(itemCount + 1, vector<long long>(capacity + 1, 0));

    //looping untuk mengisi tabel dp dengan cara 
    for (int i = 1; i <= itemCount; ++i) {
        for (int currentCapacity = 0; currentCapacity <= capacity; ++currentCapacity) {
            // Barang ke-i tidak diambil.
            dp[i][currentCapacity] = dp[i - 1][currentCapacity];
            // Jika masih muat, barang ke-i diambil. 
            if (weight[i - 1] <= currentCapacity) {
                dp[i][currentCapacity] = max(
                    dp[i][currentCapacity],
                    dp[i - 1][currentCapacity - weight[i - 1]] + value[i - 1]);
            }
        }
    }

    KnapsackResult result{dp[itemCount][capacity], 0, {}};
    int remainingCapacity = capacity;
    // Backtracking: nilai yang berbeda dari baris sebelumnya berarti barang ke-i diambil.
    for (int i = itemCount; i > 0; --i) {
        if (dp[i][remainingCapacity] != dp[i - 1][remainingCapacity]) {
            result.selectedItems.push_back(i - 1);
            result.totalWeight += weight[i - 1];
            remainingCapacity -= weight[i - 1];
        }
    }
    // Penelusuran dilakukan dari belakang sehingga urutannya dibalik.
    reverse(result.selectedItems.begin(), result.selectedItems.end());
    return result;
}

/**
 * @brief Titik masuk program: membaca input, menghitung, dan menampilkan hasil.
 *
 * Barang tanpa nama diberi nama bawaan "Barang ke-<nomor>".
 *
 * @return 0 jika berhasil, 1 jika terjadi std::invalid_argument.
 */
int main() {
    try {
        int itemCount;
        int capacity;
        cout << "Masukkan jumlah barang (N) dan kapasitas tas (W, kg): ";
        // Validasi input awal: jumlah barang dan kapasitas harus ada.
        if (!(cin >> itemCount >> capacity)) {
            throw invalid_argument("Input harus diawali jumlah barang dan kapasitas.");
        }
        if (itemCount < 0) {
            throw invalid_argument("Jumlah barang tidak boleh negatif.");
        }

        //loop untuk memasukkan data barang
        vector<int> weight(itemCount);
        vector<long long> value(itemCount);
        vector<string> name(itemCount);
        cout << "Masukkan data barang dengan format: berat(kg) nilai nama barang.\n";
        for (int i = 0; i < itemCount; ++i) {
            cout << "Barang ke-" << i + 1
                 << " (berat dalam kg, nilai, lalu nama): ";
            if (!(cin >> weight[i] >> value[i])) {
                throw invalid_argument("Data berat dan nilai barang tidak lengkap.");
            }
            // Sisa baris dianggap sebagai nama (boleh mengandung spasi).
            getline(cin, name[i]);
            const size_t firstCharacter = name[i].find_first_not_of(" \t");
            if (firstCharacter == string::npos) {
                name[i] = "Barang ke-" + to_string(i + 1);
            } else {
                name[i].erase(0, firstCharacter);
            }
        }

        // Cetak hasil knapsack
        const KnapsackResult result = knapsackBottomUp(weight, value, capacity);
        cout << "Nilai maksimal: " << result.value << '\n';
        cout << "Total berat: " << result.totalWeight << " kg\n";
        cout << "Jumlah barang terbawa: " << result.selectedItems.size() << " barang\n";
        cout << "Barang terpilih:";
        if (result.selectedItems.empty()) {
            cout << " tidak ada\n";
        } else {
            for (int index : result.selectedItems) {
                cout << " [" << name[index] << ", " << weight[index] << " kg, nilai " << value[index] << "]";
            }
            cout << '\n';
        }
        //Menangkap error 
    } catch (const invalid_argument& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
