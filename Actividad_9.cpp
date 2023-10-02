#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QTableWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QLineEdit>
#include <QVBoxLayout>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

class Neurona {
private:
    int id;
    double voltaje;
    int posX;
    int posY;
    int red;
    int green;
    int blue;

public:
    Neurona(int id, double voltaje, int posX, int posY, int red, int green, int blue)
        : id(id), voltaje(voltaje), posX(posX), posY(posY), red(red), green(green), blue(blue) {}

    friend std::ostream& operator<<(std::ostream& out, const Neurona& neurona) {
        out << "ID: " << neurona.id << ", Voltaje: " << neurona.voltaje
            << ", Posición: (" << neurona.posX << ", " << neurona.posY << ")"
            << ", Color: (" << neurona.red << ", " << neurona.green << ", " << neurona.blue << ")";
        return out;
    }
};

class NeuronaManager {
private:
    std::vector<Neurona> neuronas;

public:
    void agregarInicio(const Neurona& neurona) {
        neuronas.insert(neuronas.begin(), neurona);
    }

    void agregarFinal(const Neurona& neurona) {
        neuronas.push_back(neurona);
    }

    std::vector<Neurona> obtenerNeuronas() const {
        return neuronas;
    }
};

class NeuronaApp : public QWidget {
    Q_OBJECT

private:
    NeuronaManager manager;
    QPlainTextEdit* textEdit;
    QTableWidget* tableWidget;
    QGraphicsScene* scene;
    QGraphicsView* view;
    QLineEdit* searchLineEdit;

public:
    NeuronaApp(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout();

        QPushButton* addInicioButton = new QPushButton("Agregar al Inicio");
        QPushButton* addFinalButton = new QPushButton("Agregar al Final");
        QPushButton* mostrarButton = new QPushButton("Mostrar Neuronas");
        QPushButton* saveButton = new QPushButton("Guardar en Archivo");
        QPushButton* loadButton = new QPushButton("Cargar desde Archivo");

        textEdit = new QPlainTextEdit();
        tableWidget = new QTableWidget();
        scene = new QGraphicsScene();
        view = new QGraphicsView(scene);
        searchLineEdit = new QLineEdit();

        layout->addWidget(addInicioButton);
        layout->addWidget(addFinalButton);
        layout->addWidget(mostrarButton);
        layout->addWidget(saveButton);
        layout->addWidget(loadButton);
        layout->addWidget(textEdit);
        layout->addWidget(tableWidget);
        layout->addWidget(searchLineEdit);
        layout->addWidget(view);

        connect(addInicioButton, &QPushButton::clicked, this, &NeuronaApp::agregarAlInicio);
        connect(addFinalButton, &QPushButton::clicked, this, &NeuronaApp::agregarAlFinal);
        connect(mostrarButton, &QPushButton::clicked, this, &NeuronaApp::mostrarNeuronas);
        connect(saveButton, &QPushButton::clicked, this, &NeuronaApp::guardarEnArchivo);
        connect(loadButton, &QPushButton::clicked, this, &NeuronaApp::cargarDesdeArchivo);

        setLayout(layout);
    }

private slots:
    void agregarAlInicio() {
        // Agregar una neurona al inicio de la lista
        Neurona neurona(/* datos de la neurona */);
        manager.agregarInicio(neurona);
    }

    void agregarAlFinal() {
        // Agregar una neurona al final de la lista
        Neurona neurona(/* datos de la neurona */);
        manager.agregarFinal(neurona);
    }

    void mostrarNeuronas() {
        // Mostrar las neuronas en el QPlainTextEdit
        textEdit->clear();
        std::vector<Neurona> neuronas = manager.obtenerNeuronas();
        for (const Neurona& neurona : neuronas) {
            textEdit->appendPlainText(QString::fromStdString(neurona.toString()));
        }

        // Mostrar las neuronas en el QTableWidget
        tableWidget->setRowCount(neuronas.size());
        tableWidget->setColumnCount(7);
        tableWidget->setHorizontalHeaderLabels({"ID", "Voltaje", "PosX", "PosY", "Red", "Green", "Blue"});
        for (int i = 0; i < neuronas.size(); ++i) {
            const Neurona& neurona = neuronas[i];
            tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(neurona.id)));
            tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(neurona.voltaje)));
            tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(neurona.posX)));
            tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(neurona.posY)));
            tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(neurona.red)));
            tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(neurona.green)));
            tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(neurona.blue)));
        }

        // Dibujar las neuronas en el QScene
        scene->clear();
        for (const Neurona& neurona : neuronas) {
            scene->addEllipse(neurona.posX, neurona.posY, 20, 20, QPen(), QBrush(QColor(neurona.red, neurona.green, neurona.blue)));
        }
    }

    void guardarEnArchivo() {
        QString filePath = QFileDialog::getSaveFileName(this, "Guardar archivo", "", "Archivos de texto (*.txt)");
        if (!filePath.isEmpty()) {
            std::ofstream file(filePath.toStdString());
            std::vector<Neurona> neuronas = manager.obtenerNeuronas();
            for (const Neurona& neurona : neuronas) {
                file << neurona.id << " " << neurona.voltaje << " " << neurona.posX << " " << neurona.posY << " "
                     << neurona.red << " " << neurona.green << " " << neurona.blue << std::endl;
            }
            file.close();
        }
    }

    void cargarDesdeArchivo() {
        QString filePath = QFileDialog::getOpenFileName(this, "Cargar archivo", "", "Archivos de texto (*.txt)");
        if (!filePath.isEmpty()) {
            std::ifstream file(filePath.toStdString());
            if (file.is_open()) {
                manager.limpiarNeuronas();
                std::string line;
                while (std::getline(file, line)) {
                    std::istringstream iss(line);
                    int id, posX, posY, red, green, blue;
                    double voltaje;
                    if (iss >> id >> voltaje >> posX >> posY >> red >> green >> blue) {
                        Neurona neurona(id, voltaje, posX, posY, red, green, blue);
                        manager.agregarFinal(neurona);
                    }
                }
                file.close();
                mostrarNeuronas();
            }
        }
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    NeuronaApp mainWindow;
    mainWindow.setWindowTitle("Administrador de Neuronas");
    mainWindow.show();

    return app.exec();
}
