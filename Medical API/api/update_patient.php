<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

$id = $_POST['id'] ?? 0;
$full_name = $_POST['full_name'] ?? '';
$age = $_POST['age'] ?? null;
$gender = $_POST['gender'] ?? null;
$blood_type = $_POST['blood_type'] ?? null;
$medical_history = $_POST['medical_history'] ?? null;
$emergency_contact = $_POST['emergency_contact'] ?? null;

if (!$id || empty($full_name)) {
    echo json_encode(['success' => false, 'error' => 'ID and Full Name are required']);
    exit;
}

try {
    $stmt = $pdo->prepare("
        UPDATE patients 
        SET full_name = ?, age = ?, gender = ?, blood_type = ?, medical_history = ?, emergency_contact = ?
        WHERE id = ?
    ");

    $stmt->execute([
        $full_name,
        $age ?: null,
        $gender ?: null,
        $blood_type ?: null,
        $medical_history ?: null,
        $emergency_contact ?: null,
        $id
    ]);

    echo json_encode(['success' => true]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
