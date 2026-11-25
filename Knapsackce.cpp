/**
 * @file main.cpp
 * @brief Program penyelesaian masalah 0/1 Knapsack dengan dynamic programming.
 *
 * Program membaca jumlah barang, kapasitas tas, lalu data tiap barang
 * (berat, nilai, nama). Output berupa nilai maksimal yang bisa dibawa
 * beserta daftar barang yang dipilih.
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace std;

/**
 * @brief Hasil perhitungan knapsack.
 */
struct KnapsackResult {
    long long value;            ///< Total nilai maksimal yang bisa dicapai.
    int totalWeight;            ///< Total berat barang terpilih (kg).
    vector<int> selectedItems;  ///< Indeks barang terpilih (berbasis 0), urut naik.
};

/**
 * @brief Menyelesaikan masalah 0/1 Knapsack dengan DP bottom-up.
 *
 * Setiap barang hanya boleh diambil maksimal satu kali. Tabel
 * @c dp[i][c] menyimpan nilai maksimal yang bisa diperoleh dari @c i
 * barang pertama dengan kapasitas @c c. Setelah tabel terisi, barang
 * yang dipilih dilacak mundur (backtracking) mulai dari @c dp[N][W].
 *
 * @param weight   Berat tiap barang dalam kg. Setiap elemen harus > 0.
 * @param value    Nilai tiap barang. Ukurannya harus sama dengan @p weight.
 * @param capacity Kapasitas maksimum tas dalam kg. Tidak boleh negatif.
 * @return KnapsackResult berisi nilai maksimal, total berat, dan indeks
 *         barang terpilih.
 *
 * @throws std::invalid_argument jika @p capacity negatif, ukuran
 *         @p weight dan @p value berbeda, atau ada berat barang <= 0.
 *
 * @note Kompleksitas waktu O(N * W) dan memori O(N * W), dengan N jumlah
 *       barang dan W kapasitas. Untuk W yang sangat besar, tabel bisa
 *       memakan banyak memori.
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
    vector<vector<long long>> dp(itemCount + 1, vector<long long>(capacity + 1, 0));

    // Isi tabel: pilih antara tidak mengambil atau mengambil barang ke-i.
    for (int i = 1; i <= itemCount; ++i) {
        for (int currentCapacity = 0; currentCapacity <= capacity;
             ++currentCapacity) {
            dp[i][currentCapacity] = dp[i - 1][currentCapacity];
            if (weight[i - 1] <= currentCapacity) {
                dp[i][currentCapacity] = max(
                    dp[i][currentCapacity],
                    dp[i - 1][currentCapacity - weight[i - 1]] + value[i - 1]);
            }
        }
    }

    // Backtracking: jika nilai berubah, berarti barang ke-i diambil.
    KnapsackResult result{dp[itemCount][capacity], 0, {}};
    int remainingCapacity = capacity;
    for (int i = itemCount; i > 0; --i) {
        if (dp[i][remainingCapacity] != dp[i - 1][remainingCapacity]) {
            result.selectedItems.push_back(i - 1);
            result.totalWeight += weight[i - 1];
            remainingCapacity -= weight[i - 1];
        }
    }
    reverse(result.selectedItems.begin(), result.selectedItems.end());
    return result;
}

/**
 * @brief Titik masuk program: baca input, hitung knapsack, tampilkan hasil.
 *
 * Format input:
 * @code
 * N W
 * berat_1 nilai_1 nama barang 1
 * berat_2 nilai_2 nama barang 2
 * ...
 * @endcode
 * Nama barang boleh mengandung spasi dan boleh dikosongkan; jika kosong,
 * dipakai nama default "Barang ke-i".
 *
 * @return 0 jika berhasil, 1 jika input tidak valid.
 */
int main() {
    try {
        int itemCount;
        int capacity;
        cout << "Masukkan jumlah barang (N) dan kapasitas tas (W, kg): ";
        if (!(cin >> itemCount >> capacity)) {
            throw invalid_argument("Input harus diawali jumlah barang dan kapasitas.");
        }
        if (itemCount < 0) {
            throw invalid_argument("Jumlah barang tidak boleh negatif.");
        }

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
            // Sisa baris dibaca sebagai nama (boleh mengandung spasi).
            getline(cin, name[i]);
            const size_t firstCharacter = name[i].find_first_not_of(" \t");
            if (firstCharacter == string::npos) {
                // Nama kosong: pakai nama default.
                name[i] = "Barang ke-" + to_string(i + 1);
            } else {
                // Buang spasi di awal nama.
                name[i].erase(0, firstCharacter);
            }
        }

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
    } catch (const invalid_argument& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
