#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "token.h"
#include "parser.h"
#include <QFile>
#include <QTextStream>

void runScanner(const QString &input);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(new QGraphicsScene(this)) {
    ui->setupUi(this);


    // Attach the scene to the QGraphicsView in the UI
    ui->graphicsView->setScene(scene);
    //ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);



}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_scan_clicked() {
    // Get input from the text edit widget
    QString input = ui->input->toPlainText();

    // Run the scanner function
    runScanner(input);

    // Inform the user that tokenization is complete
    ui->label->setText("Tokenization complete. Check 'output.txt' for results.");
}

void runScanner(const QString &input) {
    QFile file("output.txt"); // Open the file 'output.txt'

    // Tokenize the input text
    QList<Token> tokens = tokenize(input);

    // Open the file for writing (overwrite the file if it exists)
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Handle the error if the file cannot be opened
        qCritical() << "Error: Unable to open file 'output.txt' for writing.";
        return;
    }

    // Create a QTextStream to write to the file
    QTextStream writer(&file);

    // Write each token to the file
    for (const Token &token : tokens) {
        writer << token.toString() << Qt::endl;
    }

    // Close the file after writing
    file.close();
}
void MainWindow::on_parser_clicked() {
    try {
        // Clear the scene before proceeding with the new parse
        scene->clear();
        QString input = ui->input->toPlainText();

        // Tokenize the input
        QList<Token> tokens = tokenize(input);

        // Parse the tokenized input
        Parser parser(tokens);
        SyntaxTreeNode* tree = parser.parse();

        // Clear the scene
        scene->clear();

        // Fixed spacing
        int xSpacing = 50; // Horizontal spacing
        int ySpacing = 60; // Vertical spacing
        int treeWidth = tree->getMaxWidth() * xSpacing; // Calculate tree width
        int treeHeight = tree->getDepth() * ySpacing;   // Calculate tree height

        // Add the tree to the scene
        int x = (treeWidth+100) / 2; // Start at the center
        tree->addToScene(scene, x, 55, xSpacing, ySpacing);

        // Adjust the scene size to fit the entire tree
        scene->setSceneRect(0, 0, treeWidth * 50, treeHeight * 50);

        // Notify the user of success
        ui->label->setText("Parsing complete. Syntax tree visualized.");
        qDebug() << "Syntax tree visualized.";

        // Clean up the tree memory
        //delete tree;
    } catch (const std::runtime_error &e) {
        // Display an error message to the user
        ui->label->setText("Parsing error.");
        qDebug() << "Error:" << e.what();
    }
}
