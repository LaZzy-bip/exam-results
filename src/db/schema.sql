PRAGMA foreign_keys = ON;

DROP TABLE IF EXISTS exam_results;
DROP TABLE IF EXISTS exams;
DROP TABLE IF EXISTS students;
DROP TABLE IF EXISTS teachers;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS subjects;
DROP TABLE IF EXISTS student_groups;
DROP TABLE IF EXISTS user_roles;

-- Роли пользователей
CREATE TABLE user_roles (
    user_role_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name         TEXT NOT NULL UNIQUE
);

-- Учебные группы
CREATE TABLE student_groups (
    group_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name     TEXT NOT NULL UNIQUE          -- например, ИВТ-4-2
);

-- Учебные дисциплины
CREATE TABLE subjects (
    subject_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name       TEXT NOT NULL UNIQUE
);

-- Учётные записи пользователей
CREATE TABLE users (
    user_id       INTEGER PRIMARY KEY AUTOINCREMENT,
    login         TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    full_name     TEXT NOT NULL,
    user_role     INTEGER NOT NULL,
    FOREIGN KEY (user_role)
        REFERENCES user_roles (user_role_id)
);

-- Данные преподавателя
CREATE TABLE teachers (
    teacher_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id    INTEGER NOT NULL UNIQUE,
    department TEXT NOT NULL,
    FOREIGN KEY (user_id)
        REFERENCES users (user_id)
        ON DELETE CASCADE
);

-- Данные студента
CREATE TABLE students (
    student_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id     INTEGER NOT NULL UNIQUE,
    group_id    INTEGER NOT NULL,
    record_book TEXT NOT NULL UNIQUE,
    FOREIGN KEY (user_id)
        REFERENCES users (user_id)
        ON DELETE CASCADE,
    FOREIGN KEY (group_id)
        REFERENCES student_groups (group_id)
);

-- Экзамен
CREATE TABLE exams (
    exam_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    subject_id INTEGER NOT NULL,
    teacher_id INTEGER NOT NULL,
    group_id   INTEGER NOT NULL,
    exam_date  TEXT NOT NULL,
    UNIQUE (subject_id, group_id, exam_date),
    FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id),
    FOREIGN KEY (teacher_id)
        REFERENCES teachers (teacher_id),
    FOREIGN KEY (group_id)
        REFERENCES student_groups (group_id)
);

-- Таблица-связка
CREATE TABLE exam_results (
    result_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    exam_id    INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    grade      INTEGER CHECK (grade BETWEEN 2 AND 5),
    UNIQUE (exam_id, student_id),
    FOREIGN KEY (exam_id)
        REFERENCES exams (exam_id)
        ON DELETE CASCADE,
    FOREIGN KEY (student_id)
        REFERENCES students (student_id)
        ON DELETE CASCADE
);

-- Тестовые данные

INSERT INTO user_roles (name)
VALUES
    ('Администратор'),
    ('Преподаватель'),
    ('Студент');

INSERT INTO student_groups (name)
VALUES
    ('ИВТ-4-1'),
    ('ИВТ-4-2');

INSERT INTO subjects (name)
VALUES
    ('Информатика'),
    ('Линейная алгебра'),
    ('История');

INSERT INTO users (login, password_hash, full_name, user_role)
VALUES
    ('admin',    '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9', 'Деканат',                   1),
    ('petrov',   'cde383eee8ee7a4400adf7a15f716f179a2eb97646b37e089eb8d6d04e663416', 'Петров Пётр Петрович',      2),
    ('uchiha', 'cde383eee8ee7a4400adf7a15f716f179a2eb97646b37e089eb8d6d04e663416', 'Учиха Саске',                 2),
    ('ivanov',   '703b0a3d6ad75b649a28adde7d83c6251da457549263bc7ff45ec709b0a8448b', 'Иванов Иван Иванович',      3),
    ('smirnova', '703b0a3d6ad75b649a28adde7d83c6251da457549263bc7ff45ec709b0a8448b', 'Смирнова Мария Андреевна',  3),
    ('litvin','703b0a3d6ad75b649a28adde7d83c6251da457549263bc7ff45ec709b0a8448b', 'Литвин Михаил Андреевич',      3),
    ('lebedeva',   '703b0a3d6ad75b649a28adde7d83c6251da457549263bc7ff45ec709b0a8448b', 'Лебедева Анна Дмитриевна',3),
    ('orlov',    '703b0a3d6ad75b649a28adde7d83c6251da457549263bc7ff45ec709b0a8448b', 'Орлов Сергей Николаевич',   3);

INSERT INTO teachers (user_id, department)
VALUES
    (2, 'Кафедра вычислительных систем и технологий'),
    (3, 'Кафедра математики');

INSERT INTO students (user_id, group_id, record_book)
VALUES
    (4, 2, '26-ИВТ-001'),
    (5, 2, '26-ИВТ-002'),
    (6, 2, '26-ИВТ-003'),
    (7, 1, '26-ИВТ-004'),
    (8, 1, '26-ИВТ-005');

INSERT INTO exams (subject_id, teacher_id, group_id, exam_date)
VALUES
    (1, 1, 2, '2027-01-21'),
    (2, 2, 2, '2027-01-24'),
    (1, 1, 1, '2027-01-28');


INSERT INTO exam_results (exam_id, student_id, grade)
VALUES
    (1, 1, 5),
    (1, 2, 4),
    (1, 3, 2),
    (2, 1, 4),
    (2, 2, NULL),
    (2, 3, 3),
    (3, 4, 3),
    (3, 5, 5);